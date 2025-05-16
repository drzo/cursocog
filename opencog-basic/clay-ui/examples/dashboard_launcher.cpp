#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#define popen _popen
#define pclose _pclose
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

/**
 * OpenCog Dashboard Launcher
 * 
 * This program serves as a launcher for the OpenCog Dashboard,
 * allowing it to connect to real AtomSpace instances and providing
 * a bridge between the GUI and the OpenCog system.
 */

// Check if a process is running by looking for its name
bool isProcessRunning(const std::string& processName) {
    std::string cmd;
#ifdef _WIN32
    cmd = "tasklist /FI \"IMAGENAME eq " + processName + "\" | findstr " + processName;
#else
    cmd = "pgrep -f " + processName;
#endif
    
    FILE* fp = popen(cmd.c_str(), "r");
    if (fp == nullptr) {
        return false;
    }
    
    char buffer[1024];
    bool running = fgets(buffer, sizeof(buffer), fp) != nullptr;
    pclose(fp);
    
    return running;
}

// Start a CogServer instance
bool startCogServer(const std::string& configFile = "") {
    std::string cmd;
#ifdef _WIN32
    cmd = "start /B cogserver";
#else
    cmd = "cogserver &";
#endif
    
    if (!configFile.empty()) {
        cmd += " -c " + configFile;
    }
    
    int result = system(cmd.c_str());
    
    // Wait a moment for the server to start
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    return result == 0 && isProcessRunning("cogserver");
}

// Start the OpenCog Dashboard
bool startDashboard() {
    std::string cmd;
#ifdef _WIN32
    cmd = "start /B dashboard.exe";
#else
    cmd = "./dashboard &";
#endif
    
    int result = system(cmd.c_str());
    
    // Wait a moment for the dashboard to start
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    return result == 0;
}

// Print welcome message
void printWelcome() {
    std::cout << "=========================================" << std::endl;
    std::cout << "OpenCog Dashboard Launcher" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "This launcher will help you start and connect to OpenCog systems." << std::endl;
    std::cout << std::endl;
}

// Main menu
void showMainMenu() {
    std::cout << "Available options:" << std::endl;
    std::cout << "1. Start OpenCog Dashboard" << std::endl;
    std::cout << "2. Start CogServer" << std::endl;
    std::cout << "3. Start both Dashboard and CogServer" << std::endl;
    std::cout << "4. Exit" << std::endl;
    std::cout << "Choose an option: ";
}

int main(int argc, char** argv) {
    printWelcome();
    
    bool running = true;
    while (running) {
        showMainMenu();
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                std::cout << "Starting OpenCog Dashboard..." << std::endl;
                if (startDashboard()) {
                    std::cout << "Dashboard started successfully." << std::endl;
                } else {
                    std::cout << "Failed to start Dashboard." << std::endl;
                }
                break;
            }
            case 2: {
                std::cout << "Starting CogServer..." << std::endl;
                if (startCogServer()) {
                    std::cout << "CogServer started successfully." << std::endl;
                } else {
                    std::cout << "Failed to start CogServer." << std::endl;
                }
                break;
            }
            case 3: {
                std::cout << "Starting CogServer and Dashboard..." << std::endl;
                bool cogServerStarted = startCogServer();
                bool dashboardStarted = startDashboard();
                
                if (cogServerStarted && dashboardStarted) {
                    std::cout << "Both started successfully." << std::endl;
                    std::cout << "You can now connect to the CogServer using the Dashboard." << std::endl;
                    std::cout << "Connection URI: cog://localhost:17001" << std::endl;
                } else {
                    if (!cogServerStarted) std::cout << "Failed to start CogServer." << std::endl;
                    if (!dashboardStarted) std::cout << "Failed to start Dashboard." << std::endl;
                }
                break;
            }
            case 4:
                running = false;
                break;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
                break;
        }
        
        std::cout << std::endl;
    }
    
    std::cout << "Exiting OpenCog Dashboard Launcher." << std::endl;
    
    return 0;
} 