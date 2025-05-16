/*
 * build_dashboard.cpp
 *
 * Copyright (C) 2025 OpenCog Foundation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "build_dashboard.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <chrono>

// ImGui for UI rendering
#include <imgui.h>
#include <GLFW/glfw3.h> // Include after ImGui

// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define PATH_SEPARATOR "\\"
#else
#include <unistd.h>
#include <sys/wait.h>
#define PATH_SEPARATOR "/"
#endif

namespace opencog {

BuildDashboard::BuildDashboard()
    : _window(nullptr),
      _selectedComponent(""),
      _numThreads(4),
      _showAdvancedOptions(false)
{
    loadComponents();
    loadSettings();
}

BuildDashboard::~BuildDashboard()
{
    saveSettings();
    
    // Stop any running build
    if (_currentBuild.isRunning) {
        stopBuild();
    }
}

void BuildDashboard::initialize(GLFWwindow* window)
{
    _window = window;
    updateComponentStatus();
}

void BuildDashboard::render()
{
    // Main build dashboard window
    ImGui::Begin("OpenCog Build Dashboard", nullptr, ImGuiWindowFlags_NoCollapse);
    
    // Left panel: Component list and build controls
    ImGui::BeginChild("LeftPanel", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, 0), true);
    renderComponentList();
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // Right panel: Build output and settings
    ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
    
    if (ImGui::BeginTabBar("BuildTabs")) {
        if (ImGui::BeginTabItem("Build Controls")) {
            renderBuildControls();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Build Output")) {
            renderBuildOutput();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Dependency Graph")) {
            renderBuildGraph();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Settings")) {
            renderSettings();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::EndChild();
    
    ImGui::End();
}

void BuildDashboard::update()
{
    // Check if builds have completed
    checkBuildStatus();
}

void BuildDashboard::startBuild(const std::string& component)
{
    // Only start a build if one isn't already running
    if (_currentBuild.isRunning) {
        return;
    }
    
    // Set up the build process
    _currentBuild.component = component;
    _currentBuild.isRunning = true;
    _currentBuild.output = "";
    _currentBuild.exitCode = 0;
    
    // Update the build output
    _buildOutput = "Starting build for " + component + "...\n";
    
    // Update component status
    if (component != "all") {
        if (_components.find(component) != _components.end()) {
            _components[component].isBuilding = true;
            _components[component].buildProgress = 0.0f;
            _components[component].buildStatus = "Building";
        }
    } else {
        for (auto& pair : _components) {
            pair.second.isBuilding = true;
            pair.second.buildProgress = 0.0f;
            pair.second.buildStatus = "Building";
        }
    }
    
    // Call the build start callback
    if (_buildStartCallback) {
        _buildStartCallback(component);
    }
    
    // Launch build in a separate thread
    std::thread([this, component]() {
        std::string cmd;
        
#ifdef _WIN32
        if (component == "all") {
            cmd = "scripts\\build.ps1";
        } else {
            cmd = "scripts\\incremental-build.ps1 -Rebuild " + component;
        }
#else
        if (component == "all") {
            cmd = "scripts/build.sh";
        } else {
            cmd = "scripts/incremental-build.sh --rebuild " + component;
        }
#endif
        
        // Execute the build command
        std::string output = executeCommand(cmd);
        
        // Update build status
        _currentBuild.isRunning = false;
        _currentBuild.output = output;
        _currentBuild.exitCode = 0; // Assume success for this demo
        
        // Append to build output
        _buildOutput += output;
        
        // Update component status
        if (component != "all") {
            if (_components.find(component) != _components.end()) {
                _components[component].isBuilding = false;
                _components[component].buildProgress = 1.0f;
                _components[component].buildStatus = (_currentBuild.exitCode == 0) ? "Success" : "Failed";
                _components[component].lastBuildTime = "Just now";
            }
        } else {
            for (auto& pair : _components) {
                pair.second.isBuilding = false;
                pair.second.buildProgress = 1.0f;
                pair.second.buildStatus = (_currentBuild.exitCode == 0) ? "Success" : "Failed";
                pair.second.lastBuildTime = "Just now";
            }
        }
        
        // Call the build complete callback
        if (_buildCompleteCallback) {
            _buildCompleteCallback(component, _currentBuild.exitCode == 0);
        }
    }).detach();
}

void BuildDashboard::stopBuild()
{
    // In a real implementation, this would kill the build process
    // For now, we'll just set the status
    
    if (!_currentBuild.isRunning) {
        return;
    }
    
    _currentBuild.isRunning = false;
    _buildOutput += "Build manually stopped.\n";
    
    // Update component status
    std::string component = _currentBuild.component;
    if (component != "all") {
        if (_components.find(component) != _components.end()) {
            _components[component].isBuilding = false;
            _components[component].buildStatus = "Stopped";
        }
    } else {
        for (auto& pair : _components) {
            if (pair.second.isBuilding) {
                pair.second.isBuilding = false;
                pair.second.buildStatus = "Stopped";
            }
        }
    }
    
    // Call the build complete callback
    if (_buildCompleteCallback) {
        _buildCompleteCallback(component, false);
    }
}

bool BuildDashboard::isBuildRunning() const
{
    return _currentBuild.isRunning;
}

void BuildDashboard::updateComponentStatus()
{
    // In a real implementation, this would query the build system for component status
    // For now, we'll simulate this
    
    // Call the build status script
    std::string output;
    
#ifdef _WIN32
    output = executeCommand("scripts\\incremental-build.ps1 -Status");
#else
    output = executeCommand("scripts/incremental-build.sh --status");
#endif
    
    // Parse the output (simplified for this demo)
    std::istringstream iss(output);
    std::string line;
    while (std::getline(iss, line)) {
        // Skip headers and empty lines
        if (line.empty() || line.find("Build Status:") != std::string::npos || 
            line.find("------------") != std::string::npos) {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string compName = line.substr(0, colonPos);
            std::string status = line.substr(colonPos + 1);
            
            // Clean up whitespace
            compName.erase(0, compName.find_first_not_of(" \t"));
            compName.erase(compName.find_last_not_of(" \t") + 1);
            status.erase(0, status.find_first_not_of(" \t"));
            status.erase(status.find_last_not_of(" \t") + 1);
            
            // Extract build status and time
            std::string buildStatus;
            std::string buildTime = "Unknown";
            
            if (status.find("Up to date") != std::string::npos) {
                buildStatus = "Success";
                
                size_t timePos = status.find("(Last built:");
                if (timePos != std::string::npos) {
                    buildTime = status.substr(timePos + 12);
                    buildTime.erase(buildTime.find_last_not_of(" \t)") + 1);
                }
            } else if (status.find("Needs rebuild") != std::string::npos) {
                buildStatus = "Needs rebuild";
                
                size_t timePos = status.find("(Last built:");
                if (timePos != std::string::npos) {
                    buildTime = status.substr(timePos + 12);
                    buildTime.erase(buildTime.find_last_not_of(" \t)") + 1);
                }
            } else if (status.find("Never built") != std::string::npos) {
                buildStatus = "Never built";
                buildTime = "Never";
            }
            
            // Update component status (if it exists)
            if (_components.find(compName) != _components.end()) {
                // Don't update if a build is in progress
                if (!_components[compName].isBuilding) {
                    _components[compName].buildStatus = buildStatus;
                    _components[compName].lastBuildTime = buildTime;
                    _components[compName].needsRebuild = (buildStatus != "Success");
                }
            }
        }
    }
}

const std::map<std::string, Component>& BuildDashboard::getComponents() const
{
    return _components;
}

void BuildDashboard::setNumThreads(int numThreads)
{
    _numThreads = numThreads;
}

int BuildDashboard::getNumThreads() const
{
    return _numThreads;
}

void BuildDashboard::setInstallDir(const std::string& dir)
{
    _installDir = dir;
}

std::string BuildDashboard::getInstallDir() const
{
    return _installDir;
}

void BuildDashboard::onComponentSelected(const std::string& component)
{
    _selectedComponent = component;
}

void BuildDashboard::setBuildStartCallback(BuildStartCallback callback)
{
    _buildStartCallback = callback;
}

void BuildDashboard::setBuildCompleteCallback(BuildCompleteCallback callback)
{
    _buildCompleteCallback = callback;
}

void BuildDashboard::renderComponentList()
{
    ImGui::Text("Components:");
    ImGui::Separator();
    
    // Build all button
    if (ImGui::Button("Build All", ImVec2(-1, 0))) {
        startBuild("all");
    }
    
    ImGui::Separator();
    
    // List of components
    for (const auto& pair : _components) {
        const Component& comp = pair.second;
        
        // Visual indicator for build status
        ImVec4 statusColor;
        if (comp.buildStatus == "Success") {
            statusColor = ImVec4(0.0f, 0.8f, 0.0f, 1.0f); // Green
        } else if (comp.buildStatus == "Building") {
            statusColor = ImVec4(0.0f, 0.0f, 0.8f, 1.0f); // Blue
        } else if (comp.buildStatus == "Failed") {
            statusColor = ImVec4(0.8f, 0.0f, 0.0f, 1.0f); // Red
        } else {
            statusColor = ImVec4(0.8f, 0.8f, 0.0f, 1.0f); // Yellow
        }
        
        ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
        
        // Add a bullet point or other visual indicator
        std::string label = "• " + comp.name;
        
        // Selectable component
        if (ImGui::Selectable(label.c_str(), _selectedComponent == comp.name)) {
            _selectedComponent = comp.name;
            onComponentSelected(comp.name);
        }
        
        // Show tooltip with component details on hover
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Component: %s", comp.name.c_str());
            ImGui::Text("Status: %s", comp.buildStatus.c_str());
            ImGui::Text("Last Build: %s", comp.lastBuildTime.c_str());
            
            if (!comp.dependencies.empty()) {
                ImGui::Text("Dependencies:");
                for (const auto& dep : comp.dependencies) {
                    ImGui::Text("  • %s", dep.c_str());
                }
            }
            
            ImGui::EndTooltip();
        }
        
        ImGui::PopStyleColor();
    }
}

void BuildDashboard::renderBuildControls()
{
    // Component details
    if (!_selectedComponent.empty()) {
        const Component& comp = _components.at(_selectedComponent);
        
        ImGui::Text("Selected Component: %s", comp.name.c_str());
        ImGui::Text("Status: %s", comp.buildStatus.c_str());
        ImGui::Text("Last Build: %s", comp.lastBuildTime.c_str());
        
        ImGui::Separator();
        
        // Dependencies
        if (!comp.dependencies.empty()) {
            ImGui::Text("Dependencies:");
            for (const auto& dep : comp.dependencies) {
                ImGui::BulletText("%s", dep.c_str());
            }
        } else {
            ImGui::Text("No dependencies");
        }
        
        ImGui::Separator();
        
        // Build actions
        if (comp.isBuilding) {
            ImGui::Text("Building...");
            ImGui::ProgressBar(comp.buildProgress);
            
            if (ImGui::Button("Stop Build", ImVec2(-1, 0))) {
                stopBuild();
            }
        } else {
            if (ImGui::Button("Build", ImVec2(-1, 0))) {
                startBuild(comp.name);
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Clean", ImVec2(-1, 0))) {
                // In a real implementation, this would clean the component
            }
        }
    } else {
        ImGui::Text("Select a component from the list");
    }
    
    // Global build controls
    ImGui::Separator();
    
    if (_currentBuild.isRunning) {
        ImGui::Text("Global build in progress...");
        
        if (ImGui::Button("Stop All Builds", ImVec2(-1, 0))) {
            stopBuild();
        }
    } else {
        if (ImGui::Button("Build All Components", ImVec2(-1, 0))) {
            startBuild("all");
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Clean All", ImVec2(-1, 0))) {
            // In a real implementation, this would clean all components
        }
    }
    
    // Build status refresh
    ImGui::Separator();
    
    if (ImGui::Button("Refresh Status", ImVec2(-1, 0))) {
        updateComponentStatus();
    }
}

void BuildDashboard::renderBuildOutput()
{
    // Build output text area
    ImGui::Text("Build Output:");
    
    ImGui::BeginChild("BuildOutputText", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::TextUnformatted(_buildOutput.c_str());
    
    // Auto-scroll to bottom if at bottom already
    static bool autoScroll = true;
    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
    
    // Auto-scroll checkbox
    ImGui::Checkbox("Auto-scroll", &autoScroll);
    
    ImGui::SameLine();
    
    if (ImGui::Button("Clear Output")) {
        _buildOutput.clear();
    }
}

void BuildDashboard::renderBuildGraph()
{
    // Simple dependency graph visualization
    ImGui::Text("Component Dependency Graph:");
    
    // In a real implementation, this would be a proper graph visualization
    // For now, just display a text-based representation
    
    ImGui::BeginChild("DependencyGraph", ImVec2(0, 0), true);
    
    for (const auto& pair : _components) {
        const Component& comp = pair.second;
        
        if (comp.dependencies.empty()) {
            ImGui::Text("%s (no dependencies)", comp.name.c_str());
        } else {
            ImGui::Text("%s depends on:", comp.name.c_str());
            
            ImGui::Indent();
            for (const auto& dep : comp.dependencies) {
                ImGui::BulletText("%s", dep.c_str());
            }
            ImGui::Unindent();
        }
        
        ImGui::Separator();
    }
    
    ImGui::EndChild();
}

void BuildDashboard::renderSettings()
{
    // Build settings
    ImGui::Text("Build Settings:");
    ImGui::Separator();
    
    // Install directory
    ImGui::Text("Installation Directory:");
    ImGui::InputText("##InstallDir", &_installDir);
    ImGui::SameLine();
    if (ImGui::Button("Browse...")) {
        // In a real implementation, this would open a directory browser
    }
    
    // Number of threads to use for building
    ImGui::SliderInt("Build Threads", &_numThreads, 1, 16);
    
    // Advanced options
    ImGui::Checkbox("Show Advanced Options", &_showAdvancedOptions);
    
    if (_showAdvancedOptions) {
        ImGui::Separator();
        ImGui::Text("Advanced Options:");
        
        static bool buildTests = false;
        ImGui::Checkbox("Build Tests", &buildTests);
        
        static bool buildDocs = false;
        ImGui::Checkbox("Build Documentation", &buildDocs);
        
        static bool useCCache = true;
        ImGui::Checkbox("Use CCache if Available", &useCCache);
    }
    
    // Save settings button
    ImGui::Separator();
    
    if (ImGui::Button("Save Settings")) {
        saveSettings();
    }
}

void BuildDashboard::checkBuildStatus()
{
    // Update progress for any running builds
    if (_currentBuild.isRunning) {
        for (auto& pair : _components) {
            Component& comp = pair.second;
            
            if (comp.isBuilding) {
                // Increment progress (in a real implementation, this would be based on actual progress)
                comp.buildProgress += 0.005f;
                if (comp.buildProgress > 1.0f) {
                    comp.buildProgress = 0.0f;
                }
            }
        }
    }
}

std::string BuildDashboard::executeCommand(const std::string& command)
{
    std::string result;
    
    // This is a simplified version for the demo
    // In a real implementation, this would properly handle process creation and output capture
    
#ifdef _WIN32
    // Windows implementation would use CreateProcess, pipes, etc.
    result = "Command output would appear here: " + command;
#else
    // Unix implementation would use popen
    result = "Command output would appear here: " + command;
#endif
    
    return result;
}

void BuildDashboard::loadComponents()
{
    // Add core components
    _components["cogutil"] = Component("cogutil");
    _components["atomspace"] = Component("atomspace");
    _components["atomspace-storage"] = Component("atomspace-storage");
    _components["atomspace-rocks"] = Component("atomspace-rocks");
    _components["atomspace-pgres"] = Component("atomspace-pgres");
    _components["cogserver"] = Component("cogserver");
    _components["learn"] = Component("learn");
    _components["opencog"] = Component("opencog");
    _components["sensory"] = Component("sensory");
    
    // Set up dependencies
    _components["atomspace"].dependencies = {"cogutil"};
    _components["atomspace-storage"].dependencies = {"cogutil", "atomspace"};
    _components["atomspace-rocks"].dependencies = {"cogutil", "atomspace", "atomspace-storage"};
    _components["atomspace-pgres"].dependencies = {"cogutil", "atomspace", "atomspace-storage"};
    _components["cogserver"].dependencies = {"cogutil", "atomspace"};
    _components["learn"].dependencies = {"cogutil", "atomspace"};
    _components["opencog"].dependencies = {"cogutil", "atomspace", "cogserver"};
    _components["sensory"].dependencies = {"cogutil", "atomspace"};
}

void BuildDashboard::loadSettings()
{
    // Set default installation directory
#ifdef _WIN32
    _installDir = "C:\\Program Files\\OpenCog";
#else
    _installDir = "/usr/local";
#endif
    
    // In a real implementation, this would load from a settings file
}

void BuildDashboard::saveSettings()
{
    // In a real implementation, this would save to a settings file
}

} // namespace opencog 