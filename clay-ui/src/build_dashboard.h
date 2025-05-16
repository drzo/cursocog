/*
 * build_dashboard.h
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

#ifndef _OPENCOG_BUILD_DASHBOARD_H
#define _OPENCOG_BUILD_DASHBOARD_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

// Forward declarations
struct GLFWwindow;

namespace opencog {

/**
 * Component information structure
 */
struct Component {
    std::string name;
    std::string buildStatus;
    std::string lastBuildTime;
    std::vector<std::string> dependencies;
    bool needsRebuild;
    bool isBuilding;
    float buildProgress;
    
    Component(const std::string& n = "") 
        : name(n), buildStatus("Unknown"), lastBuildTime("Never"), 
          needsRebuild(true), isBuilding(false), buildProgress(0.0f) {}
};

/**
 * Build process structure
 */
struct BuildProcess {
    std::string component;
    bool isRunning;
    std::string output;
    int exitCode;
    
    BuildProcess() : isRunning(false), exitCode(0) {}
};

/**
 * BuildDashboard - Visual interface for monitoring and controlling the OpenCog build process
 */
class BuildDashboard {
public:
    // Constructor and destructor
    BuildDashboard();
    ~BuildDashboard();
    
    // Initialize and run the dashboard
    void initialize(GLFWwindow* window);
    void render();
    void update();
    
    // Build control
    void startBuild(const std::string& component);
    void stopBuild();
    bool isBuildRunning() const;
    
    // Component status
    void updateComponentStatus();
    const std::map<std::string, Component>& getComponents() const;
    
    // Settings
    void setNumThreads(int numThreads);
    int getNumThreads() const;
    
    void setInstallDir(const std::string& dir);
    std::string getInstallDir() const;
    
    // Event handlers
    void onComponentSelected(const std::string& component);
    
    // Callbacks
    using BuildStartCallback = std::function<void(const std::string&)>;
    using BuildCompleteCallback = std::function<void(const std::string&, bool)>;
    
    void setBuildStartCallback(BuildStartCallback callback);
    void setBuildCompleteCallback(BuildCompleteCallback callback);
    
private:
    // Window handles
    GLFWwindow* _window;
    
    // Component data
    std::map<std::string, Component> _components;
    std::string _selectedComponent;
    
    // Build process
    BuildProcess _currentBuild;
    std::string _buildOutput;
    
    // Settings
    std::string _installDir;
    int _numThreads;
    bool _showAdvancedOptions;
    
    // Callbacks
    BuildStartCallback _buildStartCallback;
    BuildCompleteCallback _buildCompleteCallback;
    
    // UI rendering helpers
    void renderComponentList();
    void renderBuildControls();
    void renderBuildOutput();
    void renderBuildGraph();
    void renderSettings();
    
    // Build helpers
    std::string executeCommand(const std::string& command);
    void checkBuildStatus();
    
    // Data loading
    void loadComponents();
    void loadSettings();
    void saveSettings();
};

} // namespace opencog

#endif // _OPENCOG_BUILD_DASHBOARD_H 