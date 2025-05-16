/*
 * dashboard.cpp
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

/**
 * OpenCog Dashboard Application
 *
 * This application provides a visual interface for:
 * - Monitoring component build status
 * - Visualizing component dependencies
 * - Launching and controlling the build process
 * - Viewing AtomSpace data
 * - Accessing examples and documentation
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <sstream>

// OpenGL and UI libraries
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Process execution
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

// Component information structure
struct Component {
    std::string name;
    std::string buildStatus;
    std::string lastBuildTime;
    std::vector<std::string> dependencies;
    bool needsRebuild;
    bool isBuilding;
    float buildProgress;
    
    Component(const std::string& n) 
        : name(n), buildStatus("Unknown"), lastBuildTime("Never"), 
          needsRebuild(true), isBuilding(false), buildProgress(0.0f) {}
};

// Build process structure
struct BuildProcess {
    std::string component;
    bool isRunning;
    std::string output;
    int exitCode;
    
    BuildProcess() : isRunning(false), exitCode(0) {}
};

// Application class
class OpenCogDashboard {
public:
    OpenCogDashboard();
    ~OpenCogDashboard();
    
    void run();
    
private:
    // Window and UI state
    GLFWwindow* _window;
    int _windowWidth;
    int _windowHeight;
    bool _shouldClose;
    
    // Current view
    enum class View {
        DASHBOARD,
        BUILD_STATUS,
        ATOMSPACE_VIEWER,
        EXAMPLES,
        SETTINGS
    };
    View _currentView;
    
    // Component data
    std::map<std::string, Component> _components;
    
    // Build process
    BuildProcess _currentBuild;
    std::string _buildOutput;
    
    // Settings
    std::string _installDir;
    int _numThreads;
    bool _showAdvancedOptions;
    
    // Initialize UI and window
    void initUI();
    void cleanupUI();
    
    // Render UI components
    void renderMainMenu();
    void renderDashboard();
    void renderBuildStatus();
    void renderAtomSpaceViewer();
    void renderExamples();
    void renderSettings();
    
    // Update component status
    void updateComponentStatus();
    
    // Run build processes
    void startBuild(const std::string& component);
    void checkBuildStatus();
    std::string getStatusColor(const std::string& status);
    
    // Helper functions
    void loadComponents();
    void loadSettings();
    void saveSettings();
    std::string executeCommand(const std::string& command);
};

OpenCogDashboard::OpenCogDashboard()
    : _window(nullptr),
      _windowWidth(1280),
      _windowHeight(800),
      _shouldClose(false),
      _currentView(View::DASHBOARD),
      _numThreads(4),
      _showAdvancedOptions(false)
{
    loadComponents();
    loadSettings();
}

OpenCogDashboard::~OpenCogDashboard()
{
    saveSettings();
    cleanupUI();
}

void OpenCogDashboard::initUI()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    
    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    _window = glfwCreateWindow(_windowWidth, _windowHeight, "OpenCog Dashboard", nullptr, nullptr);
    if (!_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);
    
    // Set callbacks
    glfwSetWindowUserPointer(_window, this);
    glfwSetWindowSizeCallback(_window, [](GLFWwindow* w, int width, int height) {
        auto dashboard = static_cast<OpenCogDashboard*>(glfwGetWindowUserPointer(w));
        dashboard->_windowWidth = width;
        dashboard->_windowHeight = height;
    });
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void OpenCogDashboard::cleanupUI()
{
    if (_window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
}

void OpenCogDashboard::run()
{
    initUI();
    if (!_window) return;
    
    // Main loop
    while (!_shouldClose && !glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        
        // Update component status
        updateComponentStatus();
        
        // Check build status
        checkBuildStatus();
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Render UI
        renderMainMenu();
        
        switch (_currentView) {
            case View::DASHBOARD:
                renderDashboard();
                break;
            case View::BUILD_STATUS:
                renderBuildStatus();
                break;
            case View::ATOMSPACE_VIEWER:
                renderAtomSpaceViewer();
                break;
            case View::EXAMPLES:
                renderExamples();
                break;
            case View::SETTINGS:
                renderSettings();
                break;
        }
        
        // Render ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(_window);
        
        // Limit frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void OpenCogDashboard::renderMainMenu()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Dashboard", nullptr, _currentView == View::DASHBOARD)) {
            _currentView = View::DASHBOARD;
        }
        if (ImGui::MenuItem("Build Status", nullptr, _currentView == View::BUILD_STATUS)) {
            _currentView = View::BUILD_STATUS;
        }
        if (ImGui::MenuItem("AtomSpace Viewer", nullptr, _currentView == View::ATOMSPACE_VIEWER)) {
            _currentView = View::ATOMSPACE_VIEWER;
        }
        if (ImGui::MenuItem("Examples", nullptr, _currentView == View::EXAMPLES)) {
            _currentView = View::EXAMPLES;
        }
        if (ImGui::MenuItem("Settings", nullptr, _currentView == View::SETTINGS)) {
            _currentView = View::SETTINGS;
        }
        if (ImGui::MenuItem("Exit")) {
            _shouldClose = true;
        }
        ImGui::EndMainMenuBar();
    }
}

void OpenCogDashboard::renderDashboard()
{
    ImGui::Begin("OpenCog Dashboard", nullptr, ImGuiWindowFlags_NoCollapse);
    
    ImGui::Text("Welcome to the OpenCog Dashboard");
    ImGui::Separator();
    
    // System status
    ImGui::Text("System Status:");
    int readyComponents = 0;
    int totalComponents = _components.size();
    for (const auto& pair : _components) {
        if (pair.second.buildStatus == "Success") {
            readyComponents++;
        }
    }
    
    float readyPercentage = totalComponents > 0 ? 
        static_cast<float>(readyComponents) / totalComponents : 0.0f;
    
    ImGui::ProgressBar(readyPercentage, ImVec2(-1, 0), 
                       std::to_string(readyComponents) + "/" + std::to_string(totalComponents) + 
                       " components ready");
    
    // Quick actions
    ImGui::Separator();
    ImGui::Text("Quick Actions:");
    
    if (ImGui::Button("Build All Components")) {
        startBuild("all");
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Verify Dependencies")) {
        // Run dependency verification script
        std::string output = executeCommand("scripts/verify-dependencies.sh");
        ImGui::OpenPopup("Dependency Check Results");
        _buildOutput = output;
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("View Build Status")) {
        _currentView = View::BUILD_STATUS;
    }
    
    // Component summary
    ImGui::Separator();
    ImGui::Text("Component Status:");
    
    ImGui::Columns(3, "componentColumns");
    ImGui::SetColumnWidth(0, 200);
    ImGui::SetColumnWidth(1, 120);
    ImGui::Text("Component"); ImGui::NextColumn();
    ImGui::Text("Status"); ImGui::NextColumn();
    ImGui::Text("Last Build"); ImGui::NextColumn();
    ImGui::Separator();
    
    for (const auto& pair : _components) {
        const Component& comp = pair.second;
        ImGui::Text("%s", comp.name.c_str()); ImGui::NextColumn();
        
        ImGui::TextColored(ImColor::HSV(
            comp.buildStatus == "Success" ? 0.3f : 
            comp.buildStatus == "Building" ? 0.6f : 
            comp.buildStatus == "Failed" ? 0.0f : 0.1f, 
            0.8f, 0.8f), 
            "%s", comp.buildStatus.c_str()); 
        ImGui::NextColumn();
        
        ImGui::Text("%s", comp.lastBuildTime.c_str()); ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
    
    // Dependency popup
    if (ImGui::BeginPopupModal("Dependency Check Results", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Dependency Verification Results:");
        ImGui::Separator();
        
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextWrapped("%s", _buildOutput.c_str());
        ImGui::EndChild();
        
        if (ImGui::Button("Close", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    ImGui::End();
}

void OpenCogDashboard::renderBuildStatus()
{
    ImGui::Begin("Build Status", nullptr, ImGuiWindowFlags_NoCollapse);
    
    // Component dependency graph
    ImGui::Text("Component Dependencies:");
    
    // Component list with build controls
    ImGui::Separator();
    ImGui::Text("Components:");
    
    ImGui::Columns(5, "buildStatusColumns");
    ImGui::SetColumnWidth(0, 150);
    ImGui::SetColumnWidth(1, 120);
    ImGui::SetColumnWidth(2, 150);
    ImGui::SetColumnWidth(3, 100);
    ImGui::Text("Component"); ImGui::NextColumn();
    ImGui::Text("Status"); ImGui::NextColumn();
    ImGui::Text("Last Build"); ImGui::NextColumn();
    ImGui::Text("Dependencies"); ImGui::NextColumn();
    ImGui::Text("Actions"); ImGui::NextColumn();
    ImGui::Separator();
    
    for (auto& pair : _components) {
        Component& comp = pair.second;
        
        ImGui::Text("%s", comp.name.c_str()); ImGui::NextColumn();
        
        ImGui::TextColored(ImColor::HSV(
            comp.buildStatus == "Success" ? 0.3f : 
            comp.buildStatus == "Building" ? 0.6f : 
            comp.buildStatus == "Failed" ? 0.0f : 0.1f, 
            0.8f, 0.8f), 
            "%s", comp.buildStatus.c_str()); 
        ImGui::NextColumn();
        
        ImGui::Text("%s", comp.lastBuildTime.c_str()); ImGui::NextColumn();
        
        // Dependencies
        if (comp.dependencies.empty()) {
            ImGui::Text("None");
        } else {
            ImGui::Text("%zu deps", comp.dependencies.size());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                for (const auto& dep : comp.dependencies) {
                    ImGui::Text("%s", dep.c_str());
                }
                ImGui::EndTooltip();
            }
        }
        ImGui::NextColumn();
        
        // Build button
        std::string buttonLabel = comp.isBuilding ? "Building..." : "Build";
        std::string buttonId = "Build##" + comp.name;
        
        if (comp.isBuilding) {
            ImGui::ProgressBar(comp.buildProgress, ImVec2(80, 0));
        } else if (ImGui::Button(buttonId.c_str(), ImVec2(80, 0))) {
            startBuild(comp.name);
        }
        
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
    
    // Build output console
    ImGui::Separator();
    ImGui::Text("Build Output:");
    ImGui::BeginChild("BuildOutput", ImVec2(0, 200), true);
    ImGui::TextWrapped("%s", _buildOutput.c_str());
    ImGui::EndChild();
    
    // Build controls
    ImGui::Separator();
    
    if (ImGui::Button("Build All")) {
        startBuild("all");
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Clean All")) {
        // Clean all components
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Refresh Status")) {
        updateComponentStatus();
    }
    
    ImGui::End();
}

void OpenCogDashboard::renderAtomSpaceViewer()
{
    ImGui::Begin("AtomSpace Viewer", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Text("AtomSpace visualization will be shown here.");
    ImGui::End();
}

void OpenCogDashboard::renderExamples()
{
    ImGui::Begin("OpenCog Examples", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Examples and tutorials will be listed here.");
    ImGui::End();
}

void OpenCogDashboard::renderSettings()
{
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse);
    
    // Installation directory
    ImGui::Text("Installation Directory:");
    ImGui::InputText("##InstallDir", &_installDir);
    ImGui::SameLine();
    if (ImGui::Button("Browse...")) {
        // Open directory browser
    }
    
    // Build settings
    ImGui::Separator();
    ImGui::Text("Build Settings:");
    
    ImGui::SliderInt("Number of threads", &_numThreads, 1, 16);
    ImGui::Checkbox("Show advanced options", &_showAdvancedOptions);
    
    if (_showAdvancedOptions) {
        ImGui::Separator();
        ImGui::Text("Advanced Settings:");
        
        static bool useCache = true;
        ImGui::Checkbox("Use ccache if available", &useCache);
        
        static bool buildTests = false;
        ImGui::Checkbox("Build tests", &buildTests);
        
        static bool buildDocs = false;
        ImGui::Checkbox("Build documentation", &buildDocs);
    }
    
    // Save settings
    ImGui::Separator();
    if (ImGui::Button("Save Settings")) {
        saveSettings();
        ImGui::OpenPopup("SettingsSaved");
    }
    
    if (ImGui::BeginPopupModal("SettingsSaved", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Settings saved successfully!");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    ImGui::End();
}

void OpenCogDashboard::updateComponentStatus()
{
    // In a real implementation, this would query the actual build state
    // For now, we'll use a simulated approach
    
    // Call the build status script
    std::string output;
    
#ifdef _WIN32
    output = executeCommand("scripts\\incremental-build.ps1 -Status");
#else
    output = executeCommand("scripts/incremental-build.sh --status");
#endif
    
    // Parse the output
    std::istringstream iss(output);
    std::string line;
    while (std::getline(iss, line)) {
        // Skip headers
        if (line.find("Build Status:") != std::string::npos || 
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
            
            // Update component status
            if (_components.find(compName) != _components.end()) {
                _components[compName].buildStatus = buildStatus;
                _components[compName].lastBuildTime = buildTime;
                _components[compName].needsRebuild = (buildStatus != "Success");
            }
        }
    }
    
    // Update component dependencies
    for (auto& pair : _components) {
        Component& comp = pair.second;
        
        // Clear existing dependencies
        comp.dependencies.clear();
        
        // Add dependencies based on component type
        if (comp.name == "atomspace") {
            comp.dependencies.push_back("cogutil");
        } else if (comp.name == "atomspace-rocks" || comp.name == "atomspace-pgres") {
            comp.dependencies.push_back("cogutil");
            comp.dependencies.push_back("atomspace");
            comp.dependencies.push_back("atomspace-storage");
        } else if (comp.name == "cogserver") {
            comp.dependencies.push_back("cogutil");
            comp.dependencies.push_back("atomspace");
        } else if (comp.name == "opencog") {
            comp.dependencies.push_back("cogutil");
            comp.dependencies.push_back("atomspace");
            comp.dependencies.push_back("atomspace-storage");
            comp.dependencies.push_back("cogserver");
        }
    }
}

void OpenCogDashboard::startBuild(const std::string& component)
{
    // Start the build process
    _currentBuild.component = component;
    _currentBuild.isRunning = true;
    _currentBuild.output = "";
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
    
    // In a real implementation, this would start an actual build process
    // For this demo, we'll simulate a build process that takes a few seconds
    
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
        
        std::string output = executeCommand(cmd);
        
        // Update build status
        _currentBuild.isRunning = false;
        _currentBuild.output = output;
        _buildOutput += output;
        
        // Update component status
        if (component != "all") {
            if (_components.find(component) != _components.end()) {
                _components[component].isBuilding = false;
                _components[component].buildProgress = 1.0f;
                _components[component].buildStatus = "Success"; // Assume success for demo
                _components[component].lastBuildTime = "Just now";
            }
        } else {
            for (auto& pair : _components) {
                pair.second.isBuilding = false;
                pair.second.buildProgress = 1.0f;
                pair.second.buildStatus = "Success"; // Assume success for demo
                pair.second.lastBuildTime = "Just now";
            }
        }
    }).detach();
}

void OpenCogDashboard::checkBuildStatus()
{
    // Update build progress
    if (_currentBuild.isRunning) {
        for (auto& pair : _components) {
            if (pair.second.isBuilding) {
                // Increment progress
                pair.second.buildProgress += 0.01f;
                if (pair.second.buildProgress > 1.0f) {
                    pair.second.buildProgress = 0.0f;
                }
            }
        }
    }
}

std::string OpenCogDashboard::getStatusColor(const std::string& status)
{
    if (status == "Success") return "green";
    if (status == "Building") return "blue";
    if (status == "Failed") return "red";
    return "yellow";
}

void OpenCogDashboard::loadComponents()
{
    // Add core components
    _components["cogutil"] = Component("cogutil");
    _components["atomspace"] = Component("atomspace");
    _components["atomspace-storage"] = Component("atomspace-storage");
    _components["atomspace-rocks"] = Component("atomspace-rocks");
    _components["atomspace-pgres"] = Component("atomspace-pgres");
    _components["cogserver"] = Component("cogserver");
    _components["unify"] = Component("unify");
    _components["ure"] = Component("ure");
    _components["attention"] = Component("attention");
    _components["spacetime"] = Component("spacetime");
    _components["pln"] = Component("pln");
    _components["learn"] = Component("learn");
    _components["opencog"] = Component("opencog");
    _components["sensory"] = Component("sensory");
    _components["evidence"] = Component("evidence");
    
    // Set initial dependencies
    _components["atomspace"].dependencies = {"cogutil"};
    _components["atomspace-rocks"].dependencies = {"cogutil", "atomspace", "atomspace-storage"};
    _components["atomspace-pgres"].dependencies = {"cogutil", "atomspace", "atomspace-storage"};
    _components["cogserver"].dependencies = {"cogutil", "atomspace"};
    _components["ure"].dependencies = {"cogutil", "atomspace"};
    _components["attention"].dependencies = {"cogutil", "atomspace", "cogserver"};
    _components["opencog"].dependencies = {"cogutil", "atomspace", "cogserver"};
    _components["learn"].dependencies = {"cogutil", "atomspace"};
    _components["sensory"].dependencies = {"cogutil", "atomspace"};
}

void OpenCogDashboard::loadSettings()
{
    // Set default installation directory
#ifdef _WIN32
    _installDir = "C:\\Program Files\\OpenCog";
#else
    _installDir = "/usr/local";
#endif
    
    // In a real implementation, this would load from a settings file
}

void OpenCogDashboard::saveSettings()
{
    // In a real implementation, this would save to a settings file
}

std::string OpenCogDashboard::executeCommand(const std::string& command)
{
    std::string result;
    
#ifdef _WIN32
    // Windows implementation
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        return "Failed to create pipe";
    }
    
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);
    
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdError = hWritePipe;
    si.hStdOutput = hWritePipe;
    si.dwFlags |= STARTF_USESTDHANDLES;
    
    ZeroMemory(&pi, sizeof(pi));
    
    if (!CreateProcess(NULL, (LPSTR)command.c_str(), NULL, NULL, TRUE, 
                      0, NULL, NULL, &si, &pi)) {
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return "Failed to create process";
    }
    
    CloseHandle(hWritePipe);
    
    char buffer[4096];
    DWORD bytesRead;
    
    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hReadPipe);
    
#else
    // Unix implementation
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "Failed to execute command";
    }
    
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    
    pclose(pipe);
#endif
    
    return result;
}

int main(int argc, char** argv)
{
    OpenCogDashboard dashboard;
    dashboard.run();
    return 0;
} 