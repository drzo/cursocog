#define CLAY_IMPLEMENTATION
#include "../include/clay.h"
#include "../include/opencog_renderer.h"
#include "../include/opencog_atomspace_bridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

// Global state
static float g_mouseX = 0;
static float g_mouseY = 0;
static bool g_mousePressed = false;
static float g_mouseWheel = 0;
static int g_currentView = 0; // 0 = welcome, 1 = atomspace, 2 = examples, 3 = settings
static bool g_showSidebar = true;
static char g_statusMessage[256] = "Welcome to OpenCog Dashboard";

// Define colors
const Clay_Color COLOR_BACKGROUND = (Clay_Color) {25, 26, 27, 255};
const Clay_Color COLOR_PANEL = (Clay_Color) {38, 40, 42, 255};
const Clay_Color COLOR_TEXT = (Clay_Color) {220, 223, 228, 255};
const Clay_Color COLOR_HIGHLIGHT = (Clay_Color) {67, 153, 207, 255};
const Clay_Color COLOR_BUTTON = (Clay_Color) {67, 70, 75, 255};
const Clay_Color COLOR_BUTTON_HOVER = (Clay_Color) {77, 80, 85, 255};
const Clay_Color COLOR_WARNING = (Clay_Color) {230, 160, 30, 255};
const Clay_Color COLOR_ERROR = (Clay_Color) {230, 80, 80, 255};
const Clay_Color COLOR_SUCCESS = (Clay_Color) {80, 230, 120, 255};

// Forward declarations
void RenderSidebar(void);
void RenderWelcomeView(void);
void RenderAtomSpaceView(void);
void RenderExamplesView(void);
void RenderSettingsView(void);

// Error handler for Clay
void HandleClayErrors(Clay_ErrorData errorData) {
    printf("Clay Error: %.*s\n", errorData.errorText.length, errorData.errorText.chars);
}

// Main function with rendering loop
int main(int argc, char** argv) {
    printf("OpenCog Dashboard - Starting...\n");
    
    // Initialize renderer
    if (!OpenCogRenderer_Initialize(800, 600, "OpenCog Dashboard")) {
        printf("Failed to initialize renderer\n");
        return 1;
    }
    
    // Set error handler
    clay_setErrorHandler(HandleClayErrors);
    
    // Main loop
    while (OpenCogRenderer_IsRunning()) {
        // Get input
        OpenCogRenderer_GetMousePosition(&g_mouseX, &g_mouseY);
        g_mousePressed = OpenCogRenderer_IsMousePressed();
        g_mouseWheel = OpenCogRenderer_GetMouseWheel();
        
        // Begin frame
        OpenCogRenderer_BeginFrame();
        
        // Handle navigation and interaction
        {
            Clay_ElementInfo* navWelcome = Clay_GetElementByID(CLAY_ID("NavWelcome"));
            Clay_ElementInfo* navAtomSpace = Clay_GetElementByID(CLAY_ID("NavAtomSpace"));
            Clay_ElementInfo* navExamples = Clay_GetElementByID(CLAY_ID("NavExamples"));
            Clay_ElementInfo* navSettings = Clay_GetElementByID(CLAY_ID("NavSettings"));
            
            // Check navigation clicks
            if (navWelcome && navWelcome->isHovered && g_mousePressed) {
                g_currentView = 0; // Welcome view
                sprintf(g_statusMessage, "Welcome to OpenCog Dashboard");
            } else if (navAtomSpace && navAtomSpace->isHovered && g_mousePressed) {
                g_currentView = 1; // AtomSpace view
                sprintf(g_statusMessage, "AtomSpace Explorer");
            } else if (navExamples && navExamples->isHovered && g_mousePressed) {
                g_currentView = 2; // Examples view
                sprintf(g_statusMessage, "Examples & Tutorials");
            } else if (navSettings && navSettings->isHovered && g_mousePressed) {
                g_currentView = 3; // Settings view
                sprintf(g_statusMessage, "Settings & Configuration");
            }
            
            // Check quick action buttons on welcome screen
            Clay_ElementInfo* atomSpaceButton = Clay_GetElementByID(CLAY_ID("AtomSpaceButton"));
            Clay_ElementInfo* examplesButton = Clay_GetElementByID(CLAY_ID("ExamplesButton"));
            
            if (atomSpaceButton && atomSpaceButton->isHovered && g_mousePressed) {
                g_currentView = 1; // AtomSpace view
                sprintf(g_statusMessage, "AtomSpace Explorer");
            } else if (examplesButton && examplesButton->isHovered && g_mousePressed) {
                g_currentView = 2; // Examples view
                sprintf(g_statusMessage, "Examples & Tutorials");
            }
            
            // Handle Settings view interactions
            if (g_currentView == 3) {
                // Theme selection
                Clay_ElementInfo* darkThemeOption = Clay_GetElementByID(CLAY_ID("DarkThemeOption"));
                Clay_ElementInfo* lightThemeOption = Clay_GetElementByID(CLAY_ID("LightThemeOption"));
                Clay_ElementInfo* systemThemeOption = Clay_GetElementByID(CLAY_ID("SystemThemeOption"));
                
                // Font size selection
                Clay_ElementInfo* smallFontOption = Clay_GetElementByID(CLAY_ID("SmallFontOption"));
                Clay_ElementInfo* mediumFontOption = Clay_GetElementByID(CLAY_ID("MediumFontOption"));
                Clay_ElementInfo* largeFontOption = Clay_GetElementByID(CLAY_ID("LargeFontOption"));
                
                // Sidebar toggle
                Clay_ElementInfo* sidebarToggle = Clay_GetElementByID(CLAY_ID("SidebarToggle"));
                
                // Settings category selection
                Clay_ElementInfo* categoryGeneral = Clay_GetElementByID(CLAY_ID("CategoryGeneral"));
                Clay_ElementInfo* categoryAtomSpace = Clay_GetElementByID(CLAY_ID("CategoryAtomSpace"));
                Clay_ElementInfo* categoryStorage = Clay_GetElementByID(CLAY_ID("CategoryStorage"));
                Clay_ElementInfo* categoryVisualization = Clay_GetElementByID(CLAY_ID("CategoryVisualization"));
                Clay_ElementInfo* categoryAdvanced = Clay_GetElementByID(CLAY_ID("CategoryAdvanced"));
                Clay_ElementInfo* categoryAbout = Clay_GetElementByID(CLAY_ID("CategoryAbout"));
                
                // Settings actions
                Clay_ElementInfo* resetButton = Clay_GetElementByID(CLAY_ID("ResetButton"));
                Clay_ElementInfo* saveButton = Clay_GetElementByID(CLAY_ID("SaveButton"));
                
                // Theme selection
                if (darkThemeOption && darkThemeOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Dark theme selected");
                    // In a real implementation, this would update some global theme setting
                }
                if (lightThemeOption && lightThemeOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Light theme selected");
                    // In a real implementation, this would update some global theme setting
                }
                if (systemThemeOption && systemThemeOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "System theme selected");
                    // In a real implementation, this would update some global theme setting
                }
                
                // Font size selection
                if (smallFontOption && smallFontOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Small font size selected");
                    // In a real implementation, this would update some global font size setting
                }
                if (mediumFontOption && mediumFontOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Medium font size selected");
                    // In a real implementation, this would update some global font size setting
                }
                if (largeFontOption && largeFontOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Large font size selected");
                    // In a real implementation, this would update some global font size setting
                }
                
                // Sidebar toggle
                if (sidebarToggle && sidebarToggle->isHovered && g_mousePressed) {
                    g_showSidebar = !g_showSidebar;
                    sprintf(g_statusMessage, g_showSidebar ? "Sidebar enabled" : "Sidebar disabled");
                }
                
                // Settings action buttons
                if (resetButton && resetButton->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Settings reset to defaults");
                    // In a real implementation, this would reset all settings to their default values
                }
                if (saveButton && saveButton->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Settings saved");
                    // In a real implementation, this would save the settings to a configuration file
                }
            }
            
            // Handle AtomSpace view interactions
            if (g_currentView == 1) {
                // Connection button
                Clay_ElementInfo* connectButton = Clay_GetElementByID(CLAY_ID("ConnectButton"));
                
                // Storage options
                Clay_ElementInfo* rocksDBOption = Clay_GetElementByID(CLAY_ID("RocksDBOption"));
                Clay_ElementInfo* postgresOption = Clay_GetElementByID(CLAY_ID("PostgresOption"));
                Clay_ElementInfo* cogServerOption = Clay_GetElementByID(CLAY_ID("CogServerOption"));
                
                // Visualization options
                Clay_ElementInfo* forceDirectedOption = Clay_GetElementByID(CLAY_ID("ForceDirectedOption"));
                Clay_ElementInfo* hierarchicalOption = Clay_GetElementByID(CLAY_ID("HierarchicalOption"));
                
                // Refresh button
                Clay_ElementInfo* refreshButton = Clay_GetElementByID(CLAY_ID("RefreshButton"));
                
                // Storage type selection
                if (rocksDBOption && rocksDBOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "RocksDB storage selected");
                    // In a real implementation, this would update the selected storage type
                }
                if (postgresOption && postgresOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "PostgreSQL storage selected");
                    // In a real implementation, this would update the selected storage type
                }
                if (cogServerOption && cogServerOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "CogServer storage selected");
                    // In a real implementation, this would update the selected storage type
                }
                
                // Visualization layout selection
                if (forceDirectedOption && forceDirectedOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Force-directed layout selected");
                    // In a real implementation, this would update the visualization layout
                }
                if (hierarchicalOption && hierarchicalOption->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Hierarchical layout selected");
                    // In a real implementation, this would update the visualization layout
                }
                
                // Connect/disconnect button
                if (connectButton && connectButton->isHovered && g_mousePressed) {
                    // Toggle connection state
                    sprintf(g_statusMessage, "AtomSpace connection toggled");
                    // In a real implementation, this would connect to or disconnect from the AtomSpace
                }
                
                // Refresh button
                if (refreshButton && refreshButton->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Refreshing AtomSpace view");
                    // In a real implementation, this would refresh the visualization
                }
            }
            
            // Handle Examples view interactions
            if (g_currentView == 2) {
                // Category selection
                Clay_ElementInfo* categoryAtomSpaceBasics = Clay_GetElementByID(CLAY_ID("CategoryAtomSpaceBasics"));
                Clay_ElementInfo* categoryPatternMatching = Clay_GetElementByID(CLAY_ID("CategoryPatternMatching"));
                Clay_ElementInfo* categoryPersistence = Clay_GetElementByID(CLAY_ID("CategoryPersistence"));
                Clay_ElementInfo* categoryRuleEngine = Clay_GetElementByID(CLAY_ID("CategoryRuleEngine"));
                Clay_ElementInfo* categoryCogServer = Clay_GetElementByID(CLAY_ID("CategoryCogServer"));
                Clay_ElementInfo* categoryCompleteDemos = Clay_GetElementByID(CLAY_ID("CategoryCompleteDemos"));
                
                // Example open buttons
                Clay_ElementInfo* openExampleButton = Clay_GetElementByID(CLAY_ID("OpenExampleButton"));
                
                // Category selection
                if (categoryAtomSpaceBasics && categoryAtomSpaceBasics->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "AtomSpace Basics category selected");
                    // In a real implementation, this would update the displayed examples
                }
                if (categoryPatternMatching && categoryPatternMatching->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Pattern Matching category selected");
                    // In a real implementation, this would update the displayed examples
                }
                if (categoryPersistence && categoryPersistence->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Persistence category selected");
                    // In a real implementation, this would update the displayed examples
                }
                if (categoryRuleEngine && categoryRuleEngine->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Rule Engine category selected");
                    // In a real implementation, this would update the displayed examples
                }
                if (categoryCogServer && categoryCogServer->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "CogServer category selected");
                    // In a real implementation, this would update the displayed examples
                }
                if (categoryCompleteDemos && categoryCompleteDemos->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Complete Demos category selected");
                    // In a real implementation, this would update the displayed examples
                }
                
                // Open example button(s)
                if (openExampleButton && openExampleButton->isHovered && g_mousePressed) {
                    sprintf(g_statusMessage, "Opening example");
                    // In a real implementation, this would open the selected example
                }
            }
        }
        
        // Create root element
        CLAY({
            .id = CLAY_ID("Root"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .padding = CLAY_PADDING_ALL(0),
                .childGap = 0
            },
            .backgroundColor = COLOR_BACKGROUND
        }) {
            // Render sidebar if enabled
            if (g_showSidebar) {
                RenderSidebar();
            }
            
            // Render main content
            CLAY({
                .id = CLAY_ID("Content"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(0)
                }
            }) {
                // Render the current view
                switch (g_currentView) {
                    case 0:
                        RenderWelcomeView();
                        break;
                    case 1:
                        RenderAtomSpaceView();
                        break;
                    case 2:
                        RenderExamplesView();
                        break;
                    case 3:
                        RenderSettingsView();
                        break;
                }
            }
        }
        
        // End frame
        OpenCogRenderer_EndFrame();
    }
    
    // Cleanup
    OpenCogRenderer_Shutdown();
    
    return 0;
}

// Sidebar rendering
void RenderSidebar(void) {
    CLAY({
        .id = CLAY_ID("Sidebar"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(0),
            .childGap = 0
        },
        .backgroundColor = COLOR_PANEL
    }) {
        // Logo/Header
        CLAY({
            .id = CLAY_ID("Header"),
            .layout = {
                .sizing = { .height = CLAY_SIZING_FIXED(100) },
                .padding = CLAY_PADDING_ALL(16),
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = COLOR_BACKGROUND
        }) {
            CLAY_TEXT(CLAY_STRING("OpenCog\nDashboard"), CLAY_TEXT_CONFIG({ .fontSize = 20, .textColor = COLOR_TEXT }));
        }
        
        // Navigation items
        CLAY({
            .id = CLAY_ID("NavWelcome"),
            .layout = {
                .sizing = { .height = CLAY_SIZING_FIXED(50) },
                .padding = CLAY_PADDING_ALL(16),
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = g_currentView == 0 ? COLOR_HIGHLIGHT : COLOR_PANEL
        }) {
            CLAY_TEXT(CLAY_STRING("Welcome"), CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
        }
        
        CLAY({
            .id = CLAY_ID("NavAtomSpace"),
            .layout = {
                .sizing = { .height = CLAY_SIZING_FIXED(50) },
                .padding = CLAY_PADDING_ALL(16),
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = g_currentView == 1 ? COLOR_HIGHLIGHT : COLOR_PANEL
        }) {
            CLAY_TEXT(CLAY_STRING("AtomSpace"), CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
        }
        
        CLAY({
            .id = CLAY_ID("NavExamples"),
            .layout = {
                .sizing = { .height = CLAY_SIZING_FIXED(50) },
                .padding = CLAY_PADDING_ALL(16),
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = g_currentView == 2 ? COLOR_HIGHLIGHT : COLOR_PANEL
        }) {
            CLAY_TEXT(CLAY_STRING("Examples"), CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
        }
        
        CLAY({
            .id = CLAY_ID("NavSettings"),
            .layout = {
                .sizing = { .height = CLAY_SIZING_FIXED(50) },
                .padding = CLAY_PADDING_ALL(16),
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = g_currentView == 3 ? COLOR_HIGHLIGHT : COLOR_PANEL
        }) {
            CLAY_TEXT(CLAY_STRING("Settings"), CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
        }
        
        // Spacer
        CLAY({
            .id = CLAY_ID("Spacer"),
            .layout = {
                .sizing = { .height = CLAY_SIZING_GROW(1) }
            }
        }) {}
        
        // Status
        CLAY({
            .id = CLAY_ID("Status"),
            .layout = {
                .sizing = { .height = CLAY_SIZING_FIXED(30) },
                .padding = CLAY_PADDING_ALL(8),
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = COLOR_BACKGROUND
        }) {
            CLAY_TEXT(CLAY_STRING(g_statusMessage), CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_TEXT }));
        }
    }
}

// Welcome View
void RenderWelcomeView(void) {
    CLAY({
        .id = CLAY_ID("WelcomeView"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(32),
            .childGap = 24
        }
    }) {
        // Header
        CLAY({
            .id = CLAY_ID("WelcomeHeader"),
            .layout = {
                .padding = CLAY_PADDING_ALL(0),
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            }
        }) {
            CLAY_TEXT(CLAY_STRING("Welcome to OpenCog Basic"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 28, .textColor = COLOR_TEXT }));
        }
        
        // Description
        CLAY({
            .id = CLAY_ID("WelcomeDescription"),
            .layout = {
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("OpenCog Basic provides the core components of the OpenCog framework:"),
                    CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
            
            CLAY_TEXT(CLAY_STRING("• AtomSpace - Knowledge representation framework\n"
                                "• AtomSpace Storage - Persistence backends\n"
                                "• CogServer - Network server for AtomSpace\n"
                                "• Clay UI - Visualization tools"),
                    CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
        }
        
        // Quick Start
        CLAY({
            .id = CLAY_ID("QuickStart"),
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Quick Start Guide"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 20, .textColor = COLOR_HIGHLIGHT }));
            
            // Steps
            CLAY({
                .id = CLAY_ID("QuickStartSteps"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .padding = CLAY_PADDING_ALL(8),
                    .childGap = 16
                }
            }) {
                CLAY_TEXT(CLAY_STRING("1. Navigate to the AtomSpace tab to explore the knowledge graph"),
                        CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                
                CLAY_TEXT(CLAY_STRING("2. Visit the Examples tab to see sample code and usage patterns"),
                        CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                
                CLAY_TEXT(CLAY_STRING("3. Configure your environment in the Settings tab"),
                        CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
            }
        }
        
        // Quick Actions
        CLAY({
            .id = CLAY_ID("QuickActions"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .padding = CLAY_PADDING_ALL(0),
                .childGap = 16,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            }
        }) {
            // AtomSpace Button
            CLAY({
                .id = CLAY_ID("AtomSpaceButton"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIXED(180), .height = CLAY_SIZING_FIXED(40) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Explore AtomSpace"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
            }
            
            // Examples Button
            CLAY({
                .id = CLAY_ID("ExamplesButton"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIXED(180), .height = CLAY_SIZING_FIXED(40) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("View Examples"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
            }
        }
        
        // Version Info
        CLAY({
            .id = CLAY_ID("VersionInfo"),
            .layout = {
                .padding = CLAY_PADDING_ALL(0),
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            }
        }) {
            CLAY_TEXT(CLAY_STRING("OpenCog Basic v0.1.0"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = (Clay_Color){150, 150, 150, 255} }));
        }
    }
}

// AtomSpace View
void RenderAtomSpaceView(void) {
    CLAY({
        .id = CLAY_ID("AtomSpaceView"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
        // Header with controls
        CLAY({
            .id = CLAY_ID("AtomSpaceHeader"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .height = CLAY_SIZING_FIXED(50) },
                .padding = CLAY_PADDING_ALL(8),
                .childGap = 16,
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("AtomSpace Explorer"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 20, .textColor = COLOR_TEXT }));
            
            // Spacer
            CLAY({
                .id = CLAY_ID("HeaderSpacer"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(1) }
                }
            }) {}
            
            // Connection indicator
            CLAY({
                .id = CLAY_ID("ConnectionIndicator"),
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .childGap = 8,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                CLAY({
                    .id = CLAY_ID("StatusDot"),
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIXED(12), .height = CLAY_SIZING_FIXED(12) }
                    },
                    .backgroundColor = COLOR_WARNING, // Change to COLOR_SUCCESS when connected
                    .cornerRadius = CLAY_CORNER_RADIUS(6)
                }) {}
                
                CLAY_TEXT(CLAY_STRING("Not Connected"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_WARNING }));
            }
            
            // Connect button
            CLAY({
                .id = CLAY_ID("ConnectButton"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Connect"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
        }
        
        // Main content area - split into sidebar and graph view
        CLAY({
            .id = CLAY_ID("AtomSpaceContent"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .height = CLAY_SIZING_GROW(1) },
                .childGap = 16
            }
        }) {
            // Left sidebar with controls
            CLAY({
                .id = CLAY_ID("AtomSpaceControlSidebar"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_FIXED(250), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 16
                },
                .backgroundColor = COLOR_PANEL,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                // Storage type selection
                CLAY({
                    .id = CLAY_ID("StorageSection"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .childGap = 8
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("Storage Type:"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                    
                    // Storage options
                    CLAY({
                        .id = CLAY_ID("StorageOptions"),
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .childGap = 8
                        }
                    }) {
                        // RocksDB option
                        CLAY({
                            .id = CLAY_ID("RocksDBOption"),
                            .layout = {
                                .padding = CLAY_PADDING_ALL(8),
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = COLOR_HIGHLIGHT, // Highlight selected option
                            .cornerRadius = CLAY_CORNER_RADIUS(4)
                        }) {
                            CLAY_TEXT(CLAY_STRING("RocksDB"), 
                                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        }
                        
                        // Postgres option
                        CLAY({
                            .id = CLAY_ID("PostgresOption"),
                            .layout = {
                                .padding = CLAY_PADDING_ALL(8),
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = COLOR_BACKGROUND,
                            .cornerRadius = CLAY_CORNER_RADIUS(4)
                        }) {
                            CLAY_TEXT(CLAY_STRING("PostgreSQL"), 
                                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        }
                        
                        // CogServer option
                        CLAY({
                            .id = CLAY_ID("CogServerOption"),
                            .layout = {
                                .padding = CLAY_PADDING_ALL(8),
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = COLOR_BACKGROUND,
                            .cornerRadius = CLAY_CORNER_RADIUS(4)
                        }) {
                            CLAY_TEXT(CLAY_STRING("CogServer"), 
                                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        }
                    }
                }
                
                // URI input
                CLAY({
                    .id = CLAY_ID("URISection"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .childGap = 8
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("URI:"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                    
                    CLAY({
                        .id = CLAY_ID("URIInput"),
                        .layout = {
                            .padding = CLAY_PADDING_ALL(8),
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = COLOR_BACKGROUND,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        CLAY_TEXT(CLAY_STRING("rocks:///tmp/atomspace-rocks"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                    }
                }
                
                // Visualization controls
                CLAY({
                    .id = CLAY_ID("VisualizationSection"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .childGap = 8
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("Visualization:"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                    
                    // Layout type
                    CLAY({
                        .id = CLAY_ID("LayoutType"),
                        .layout = {
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .childGap = 8
                        }
                    }) {
                        // Force-directed option
                        CLAY({
                            .id = CLAY_ID("ForceDirectedOption"),
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_FIXED(110), .height = CLAY_SIZING_FIXED(34) },
                                .padding = CLAY_PADDING_ALL(8),
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = COLOR_HIGHLIGHT,
                            .cornerRadius = CLAY_CORNER_RADIUS(4)
                        }) {
                            CLAY_TEXT(CLAY_STRING("Force"), 
                                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        }
                        
                        // Hierarchical option
                        CLAY({
                            .id = CLAY_ID("HierarchicalOption"),
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_FIXED(110), .height = CLAY_SIZING_FIXED(34) },
                                .padding = CLAY_PADDING_ALL(8),
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = COLOR_BUTTON,
                            .cornerRadius = CLAY_CORNER_RADIUS(4)
                        }) {
                            CLAY_TEXT(CLAY_STRING("Hierarchical"), 
                                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        }
                    }
                }
                
                // Filter options
                CLAY({
                    .id = CLAY_ID("FilterSection"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .childGap = 8
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("Filters:"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                    
                    // Atom type filter
                    CLAY({
                        .id = CLAY_ID("AtomTypeFilter"),
                        .layout = {
                            .padding = CLAY_PADDING_ALL(8),
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = COLOR_BACKGROUND,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        CLAY_TEXT(CLAY_STRING("All Atom Types"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                    }
                    
                    // Depth filter
                    CLAY({
                        .id = CLAY_ID("DepthFilter"),
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .childGap = 4
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Depth: 3"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        
                        // Slider representation
                        CLAY({
                            .id = CLAY_ID("DepthSlider"),
                            .layout = {
                                .sizing = { .height = CLAY_SIZING_FIXED(4) }
                            },
                            .backgroundColor = COLOR_BUTTON,
                            .cornerRadius = CLAY_CORNER_RADIUS(2)
                        }) {
                            // Slider handle
                            CLAY({
                                .id = CLAY_ID("SliderHandle"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(16), .height = CLAY_SIZING_FIXED(16) },
                                    .positioning = { .positioningMode = CLAY_POSITIONING_ABSOLUTE, .position = {.x = 110, .y = -6} }
                                },
                                .backgroundColor = COLOR_HIGHLIGHT,
                                .cornerRadius = CLAY_CORNER_RADIUS(8)
                            }) {}
                        }
                    }
                }
                
                // Spacer
                CLAY({
                    .id = CLAY_ID("ControlsSpacer"),
                    .layout = {
                        .sizing = { .height = CLAY_SIZING_GROW(1) }
                    }
                }) {}
                
                // Refresh button
                CLAY({
                    .id = CLAY_ID("RefreshButton"),
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(40) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BUTTON,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Refresh View"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
            }
            
            // Main graph view
            CLAY({
                .id = CLAY_ID("AtomSpaceGraphContainer"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(16),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_BACKGROUND,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                // Placeholder for graph visualization
                CLAY_TEXT(CLAY_STRING("AtomSpace Visualization\n\nConnect to an AtomSpace to view atoms"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT }));
            }
        }
        
        // Details panel for selected atom
        CLAY({
            .id = CLAY_ID("AtomDetailsPanel"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .height = CLAY_SIZING_FIXED(150) },
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY({
                .id = CLAY_ID("AtomDetails"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .childGap = 8
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Selected Atom:"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                
                CLAY_TEXT(CLAY_STRING("No atom selected"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = (Clay_Color){150, 150, 150, 255} }));
                
                CLAY_TEXT(CLAY_STRING("Click on an atom in the visualization to view details."), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = (Clay_Color){150, 150, 150, 255} }));
            }
        }
    }
}

// Examples View
void RenderExamplesView(void) {
    CLAY({
        .id = CLAY_ID("ExamplesView"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
        // Header
        CLAY({
            .id = CLAY_ID("ExamplesHeader"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .height = CLAY_SIZING_FIXED(50) },
                .padding = CLAY_PADDING_ALL(8),
                .childGap = 16,
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Examples & Tutorials"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 20, .textColor = COLOR_TEXT }));
            
            // Spacer
            CLAY({
                .id = CLAY_ID("HeaderSpacer"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(1) }
                }
            }) {}
            
            // Search box (placeholder)
            CLAY({
                .id = CLAY_ID("SearchBox"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_FIXED(34) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_BACKGROUND,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Search examples..."), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = (Clay_Color){150, 150, 150, 255} }));
            }
        }
        
        // Main content area - split into categories and content
        CLAY({
            .id = CLAY_ID("ExamplesContent"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .height = CLAY_SIZING_GROW(1) },
                .childGap = 16
            }
        }) {
            // Categories sidebar
            CLAY({
                .id = CLAY_ID("CategoriesSidebar"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 12
                },
                .backgroundColor = COLOR_PANEL,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Categories"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                
                // Category options
                // AtomSpace Basics (selected)
                CLAY({
                    .id = CLAY_ID("CategoryAtomSpaceBasics"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_HIGHLIGHT,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("AtomSpace Basics"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // Pattern Matching
                CLAY({
                    .id = CLAY_ID("CategoryPatternMatching"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Pattern Matching"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // Persistence
                CLAY({
                    .id = CLAY_ID("CategoryPersistence"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Persistence"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // Rule Engine
                CLAY({
                    .id = CLAY_ID("CategoryRuleEngine"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Rule Engine"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // CogServer
                CLAY({
                    .id = CLAY_ID("CategoryCogServer"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("CogServer"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // Complete Demos
                CLAY({
                    .id = CLAY_ID("CategoryCompleteDemos"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Complete Demos"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
            }
            
            // Example content area
            CLAY({
                .id = CLAY_ID("ExampleContentArea"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(0),
                    .childGap = 16
                }
            }) {
                // Category title and description
                CLAY({
                    .id = CLAY_ID("CategoryDescription"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .padding = CLAY_PADDING_ALL(16),
                        .childGap = 8
                    },
                    .backgroundColor = COLOR_PANEL,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("AtomSpace Basics"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_HIGHLIGHT }));
                    
                    CLAY_TEXT(CLAY_STRING("Learn the fundamentals of working with the AtomSpace, including creating atoms, building relationships, and traversing the knowledge graph."), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // Example cards container
                CLAY({
                    .id = CLAY_ID("ExampleCards"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .childGap = 16
                    }
                }) {
                    // Example 1
                    CLAY({
                        .id = CLAY_ID("Example1"),
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .padding = CLAY_PADDING_ALL(16),
                            .childGap = 12
                        },
                        .backgroundColor = COLOR_PANEL,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        CLAY_TEXT(CLAY_STRING("Creating Your First Atoms"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                        
                        CLAY_TEXT(CLAY_STRING("Learn how to create Nodes and Links in the AtomSpace."), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        
                        // Tags and buttons
                        CLAY({
                            .id = CLAY_ID("Example1Footer"),
                            .layout = {
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                .childGap = 8,
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            // Difficulty tag
                            CLAY({
                                .id = CLAY_ID("DifficultyTag"),
                                .layout = {
                                    .padding = CLAY_PADDING(4, 8, 4, 8),
                                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BACKGROUND,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Beginner"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_SUCCESS }));
                            }
                            
                            // Duration tag
                            CLAY({
                                .id = CLAY_ID("DurationTag"),
                                .layout = {
                                    .padding = CLAY_PADDING(4, 8, 4, 8),
                                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BACKGROUND,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("10 min"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_TEXT }));
                            }
                            
                            // Spacer
                            CLAY({
                                .id = CLAY_ID("FooterSpacer"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_GROW(1) }
                                }
                            }) {}
                            
                            // Open button
                            CLAY({
                                .id = CLAY_ID("OpenExampleButton"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(30) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BUTTON,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Open"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_TEXT }));
                            }
                        }
                    }
                    
                    // Example 2
                    CLAY({
                        .id = CLAY_ID("Example2"),
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .padding = CLAY_PADDING_ALL(16),
                            .childGap = 12
                        },
                        .backgroundColor = COLOR_PANEL,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        CLAY_TEXT(CLAY_STRING("Working with Truth Values"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                        
                        CLAY_TEXT(CLAY_STRING("Understanding and manipulating truth values in AtomSpace."), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        
                        // Tags and buttons
                        CLAY({
                            .id = CLAY_ID("Example2Footer"),
                            .layout = {
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                .childGap = 8,
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            // Difficulty tag
                            CLAY({
                                .id = CLAY_ID("DifficultyTag"),
                                .layout = {
                                    .padding = CLAY_PADDING(4, 8, 4, 8),
                                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BACKGROUND,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Intermediate"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_WARNING }));
                            }
                            
                            // Duration tag
                            CLAY({
                                .id = CLAY_ID("DurationTag"),
                                .layout = {
                                    .padding = CLAY_PADDING(4, 8, 4, 8),
                                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BACKGROUND,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("15 min"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_TEXT }));
                            }
                            
                            // Spacer
                            CLAY({
                                .id = CLAY_ID("FooterSpacer"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_GROW(1) }
                                }
                            }) {}
                            
                            // Open button
                            CLAY({
                                .id = CLAY_ID("OpenExampleButton"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(30) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BUTTON,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Open"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_TEXT }));
                            }
                        }
                    }
                    
                    // Example 3
                    CLAY({
                        .id = CLAY_ID("Example3"),
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .padding = CLAY_PADDING_ALL(16),
                            .childGap = 12
                        },
                        .backgroundColor = COLOR_PANEL,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        CLAY_TEXT(CLAY_STRING("Querying the AtomSpace"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                        
                        CLAY_TEXT(CLAY_STRING("Learn basic techniques for finding and retrieving atoms."), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        
                        // Tags and buttons
                        CLAY({
                            .id = CLAY_ID("Example3Footer"),
                            .layout = {
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                .childGap = 8,
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            // Difficulty tag
                            CLAY({
                                .id = CLAY_ID("DifficultyTag"),
                                .layout = {
                                    .padding = CLAY_PADDING(4, 8, 4, 8),
                                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BACKGROUND,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Beginner"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_SUCCESS }));
                            }
                            
                            // Duration tag
                            CLAY({
                                .id = CLAY_ID("DurationTag"),
                                .layout = {
                                    .padding = CLAY_PADDING(4, 8, 4, 8),
                                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BACKGROUND,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("20 min"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_TEXT }));
                            }
                            
                            // Spacer
                            CLAY({
                                .id = CLAY_ID("FooterSpacer"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_GROW(1) }
                                }
                            }) {}
                            
                            // Open button
                            CLAY({
                                .id = CLAY_ID("OpenExampleButton"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(30) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BUTTON,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Open"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 12, .textColor = COLOR_TEXT }));
                            }
                        }
                    }
                }
            }
        }
    }
}

// Settings View
void RenderSettingsView(void) {
    CLAY({
        .id = CLAY_ID("SettingsView"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
        // Header
        CLAY({
            .id = CLAY_ID("SettingsHeader"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .height = CLAY_SIZING_FIXED(50) },
                .padding = CLAY_PADDING_ALL(8),
                .childGap = 16,
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Settings & Configuration"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 20, .textColor = COLOR_TEXT }));
        }
        
        // Main content area with settings categories
        CLAY({
            .id = CLAY_ID("SettingsContent"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .height = CLAY_SIZING_GROW(1) },
                .childGap = 16
            }
        }) {
            // Settings categories sidebar
            CLAY({
                .id = CLAY_ID("SettingsCategoriesSidebar"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 12
                },
                .backgroundColor = COLOR_PANEL,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Categories"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                
                // Category options
                // General Settings (selected)
                CLAY({
                    .id = CLAY_ID("CategoryGeneral"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_HIGHLIGHT,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("General"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // AtomSpace Settings
                CLAY({
                    .id = CLAY_ID("CategoryAtomSpace"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("AtomSpace"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // Storage Settings
                CLAY({
                    .id = CLAY_ID("CategoryStorage"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Storage"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // Visualization Settings
                CLAY({
                    .id = CLAY_ID("CategoryVisualization"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Visualization"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // Advanced Settings
                CLAY({
                    .id = CLAY_ID("CategoryAdvanced"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Advanced"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
                
                // About
                CLAY({
                    .id = CLAY_ID("CategoryAbout"),
                    .layout = {
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("About"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
            }
            
            // Settings content area (showing General settings)
            CLAY({
                .id = CLAY_ID("SettingsContentArea"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(0),
                    .childGap = 16
                }
            }) {
                // Settings group - Appearance
                CLAY({
                    .id = CLAY_ID("AppearanceSettings"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .padding = CLAY_PADDING_ALL(16),
                        .childGap = 16
                    },
                    .backgroundColor = COLOR_PANEL,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Appearance"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_HIGHLIGHT }));
                    
                    // Theme setting
                    CLAY({
                        .id = CLAY_ID("ThemeSetting"),
                        .layout = {
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .childGap = 16,
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Theme:"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        
                        // Theme options
                        CLAY({
                            .id = CLAY_ID("ThemeOptions"),
                            .layout = {
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                .childGap = 8
                            }
                        }) {
                            // Dark theme (selected)
                            CLAY({
                                .id = CLAY_ID("DarkThemeOption"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_HIGHLIGHT,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Dark"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                            
                            // Light theme
                            CLAY({
                                .id = CLAY_ID("LightThemeOption"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BUTTON,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Light"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                            
                            // System theme
                            CLAY({
                                .id = CLAY_ID("SystemThemeOption"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BUTTON,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("System"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                        }
                    }
                    
                    // Font size setting
                    CLAY({
                        .id = CLAY_ID("FontSizeSetting"),
                        .layout = {
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .childGap = 16,
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Font Size:"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        
                        // Font size options
                        CLAY({
                            .id = CLAY_ID("FontSizeOptions"),
                            .layout = {
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                .childGap = 8
                            }
                        }) {
                            // Small
                            CLAY({
                                .id = CLAY_ID("SmallFontOption"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BUTTON,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Small"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                            
                            // Medium (selected)
                            CLAY({
                                .id = CLAY_ID("MediumFontOption"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_HIGHLIGHT,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Medium"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                            
                            // Large
                            CLAY({
                                .id = CLAY_ID("LargeFontOption"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BUTTON,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Large"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                        }
                    }
                    
                    // Show sidebar toggle
                    CLAY({
                        .id = CLAY_ID("SidebarSetting"),
                        .layout = {
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .childGap = 16,
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Show Sidebar:"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        
                        // Toggle switch
                        CLAY({
                            .id = CLAY_ID("SidebarToggle"),
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_FIXED(50), .height = CLAY_SIZING_FIXED(24) },
                                .padding = CLAY_PADDING_ALL(2)
                            },
                            .backgroundColor = COLOR_SUCCESS,
                            .cornerRadius = CLAY_CORNER_RADIUS(12)
                        }) {
                            // Toggle handle (positioned to right to show "enabled")
                            CLAY({
                                .id = CLAY_ID("ToggleHandle"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(20), .height = CLAY_SIZING_FIXED(20) },
                                    .positioning = { .positioningMode = CLAY_POSITIONING_ABSOLUTE, .position = {.x = 28, .y = 2} }
                                },
                                .backgroundColor = COLOR_TEXT,
                                .cornerRadius = CLAY_CORNER_RADIUS(10)
                            }) {}
                        }
                    }
                }
                
                // Settings group - Paths
                CLAY({
                    .id = CLAY_ID("PathsSettings"),
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .padding = CLAY_PADDING_ALL(16),
                        .childGap = 16
                    },
                    .backgroundColor = COLOR_PANEL,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Paths & Directories"), 
                            CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_HIGHLIGHT }));
                    
                    // Default storage location
                    CLAY({
                        .id = CLAY_ID("StoragePathSetting"),
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .childGap = 8
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Default Storage Location:"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        
                        // Path input and browse button
                        CLAY({
                            .id = CLAY_ID("StoragePathInput"),
                            .layout = {
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                .childGap = 8,
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            // Path input field
                            CLAY({
                                .id = CLAY_ID("PathField"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BACKGROUND,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("/tmp/atomspace-rocks"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                            
                            // Browse button
                            CLAY({
                                .id = CLAY_ID("BrowseButton"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(80), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BUTTON,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Browse"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                        }
                    }
                    
                    // Examples path
                    CLAY({
                        .id = CLAY_ID("ExamplesPathSetting"),
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .childGap = 8
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Examples Directory:"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                        
                        // Path input and browse button
                        CLAY({
                            .id = CLAY_ID("ExamplesPathInput"),
                            .layout = {
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                .childGap = 8,
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            // Path input field
                            CLAY({
                                .id = CLAY_ID("PathField"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_GROW(1), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BACKGROUND,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("./examples"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                            
                            // Browse button
                            CLAY({
                                .id = CLAY_ID("BrowseButton"),
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(80), .height = CLAY_SIZING_FIXED(34) },
                                    .padding = CLAY_PADDING_ALL(8),
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = COLOR_BUTTON,
                                .cornerRadius = CLAY_CORNER_RADIUS(4)
                            }) {
                                CLAY_TEXT(CLAY_STRING("Browse"), 
                                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                            }
                        }
                    }
                }
                
                // Settings actions
                CLAY({
                    .id = CLAY_ID("SettingsActions"),
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .childGap = 16,
                        .childAlignment = { .x = CLAY_ALIGN_X_RIGHT }
                    }
                }) {
                    // Reset to defaults button
                    CLAY({
                        .id = CLAY_ID("ResetButton"),
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(150), .height = CLAY_SIZING_FIXED(40) },
                            .padding = CLAY_PADDING_ALL(8),
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = COLOR_BUTTON,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        CLAY_TEXT(CLAY_STRING("Reset to Defaults"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                    }
                    
                    // Save button
                    CLAY({
                        .id = CLAY_ID("SaveButton"),
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIXED(40) },
                            .padding = CLAY_PADDING_ALL(8),
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = COLOR_SUCCESS,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        CLAY_TEXT(CLAY_STRING("Save Settings"), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                    }
                }
            }
        }
    }
} 