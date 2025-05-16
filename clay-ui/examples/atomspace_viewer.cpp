#define CLAY_IMPLEMENTATION#include "../include/clay.h"#include "../include/opencog_renderer.h"#include "../include/opencog_atomspace_bridge.h"#include <stdio.h>#include <stdlib.h>#include <string>// Global statestatic float g_mouseX = 0;static float g_mouseY = 0;static bool g_mousePressed = false;static float g_mouseWheel = 0;static bool g_atomSpaceConnected = false;static const char* g_selectedAtomId = nullptr;static int g_currentView = 0; // 0 = graph, 1 = query, 2 = settingsstatic char g_storageType[64] = "rocks"; // Default storage typestatic char g_storageURI[256] = "rocks:///tmp/atomspace-rocks"; // Default URI// Define storage optionstypedef struct {    const char* name;    const char* description;    const char* default_uri;} StorageOption;const StorageOption STORAGE_OPTIONS[] = {    {"rocks", "RocksDB local storage", "rocks:///tmp/atomspace-rocks"},    {"cogserver", "CogServer network storage", "cog://localhost:17001"},    {"postgres", "PostgreSQL database storage", "postgres://localhost/atomspace?user=opencog&password=password"}};const int NUM_STORAGE_OPTIONS = sizeof(STORAGE_OPTIONS) / sizeof(StorageOption);

// Define storage options
typedef struct {
    const char* name;
    const char* description;
    const char* default_uri;
} StorageOption;

const StorageOption STORAGE_OPTIONS[] = {
    {"rocks", "RocksDB local storage", "rocks:///tmp/atomspace-rocks"},
    {"cogserver", "CogServer network storage", "cog://localhost:17001"},
    {"postgres", "PostgreSQL database storage", "postgres://localhost/atomspace?user=opencog&password=password"}
};
const int NUM_STORAGE_OPTIONS = sizeof(STORAGE_OPTIONS) / sizeof(StorageOption);

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

// Error handler for Clay
void HandleClayErrors(Clay_ErrorData errorData) {
    printf("Clay Error: %.*s\n", errorData.errorText.length, errorData.errorText.chars);
}

// Helper function to render a button
bool RenderButton(const char* text, Clay_BoundingBox bbox) {
    // Check if mouse is hovering
    bool hover = g_mouseX >= bbox.x && g_mouseX < bbox.x + bbox.width &&
                g_mouseY >= bbox.y && g_mouseY < bbox.y + bbox.height;
    
    // Check if clicked
    bool clicked = hover && g_mousePressed;
    
    // Draw button
    Clay_Color buttonColor = hover ? COLOR_BUTTON_HOVER : COLOR_BUTTON;
    
    // Return if clicked
    return clicked;
}

// Callback for atom selection
void OnAtomSelected(void* atomPtr, void* userData) {
    printf("Atom selected: %p\n", atomPtr);
    
    // In a real implementation, we would get the atom ID and update the UI
}

// Connect to an AtomSpace
bool ConnectToAtomSpace(const char* storageType, const char* uri) {
    // Initialize the bridge if not already
    if (!g_atomSpaceConnected) {
        if (!OpenCogAtomSpaceBridge_Initialize(storageType, uri)) {
            printf("Failed to initialize AtomSpace bridge\n");
            return false;
        }
    }
    
    // Connect to the AtomSpace
    if (!OpenCogAtomSpaceBridge_Connect(storageType, uri)) {
        printf("Failed to connect to AtomSpace\n");
        return false;
    }
    
    // Set up a monitor for atom changes
    OpenCogAtomSpaceBridge_AddChangeMonitor(OnAtomSelected, nullptr);
    
    // Mark as connected
    g_atomSpaceConnected = true;
    
    // Update the storageType and URI
    strncpy(g_storageType, storageType, sizeof(g_storageType) - 1);
    strncpy(g_storageURI, uri, sizeof(g_storageURI) - 1);
    
    return true;
}

// Disconnect from the AtomSpace
bool DisconnectFromAtomSpace() {
    if (!g_atomSpaceConnected) {
        return true;
    }
    
    // Disconnect from the AtomSpace
    OpenCogAtomSpaceBridge_Disconnect();
    
    // Mark as disconnected
    g_atomSpaceConnected = false;
    
    return true;
}

// Render the toolbar
void RenderToolbar(void) {
    CLAY({
        .id = CLAY_ID("Toolbar"),
        .layout = { 
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) },
            .padding = CLAY_PADDING_ALL(8),
            .childGap = 8,
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = COLOR_PANEL
    }) {
        // Logo/title
        CLAY_TEXT(CLAY_STRING("AtomSpace Viewer"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT }));
        
        // Connection status
        CLAY({
            .id = CLAY_ID("ConnectionStatus"),
            .layout = { 
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 8,
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            }
        }) {
            CLAY_TEXT(
                CLAY_STRING(g_atomSpaceConnected ? 
                    g_storageType : "Not Connected"), 
                CLAY_TEXT_CONFIG({ 
                    .fontSize = 14, 
                    .textColor = g_atomSpaceConnected ? COLOR_SUCCESS : COLOR_WARNING 
                })
            );
        }
        
        // Spacer
        CLAY({
            .id = CLAY_ID("Spacer"),
            .layout = { .sizing = { .width = CLAY_SIZING_GROW(1) } }
        }) {}
        
        // View buttons
        CLAY({
            .id = CLAY_ID("ViewButtons"),
            .layout = { 
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 4
            }
        }) {
            // Graph view button
            CLAY({
                .id = CLAY_ID("GraphViewButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = g_currentView == 0 ? COLOR_HIGHLIGHT : COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Graph"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
            
            // Query view button
            CLAY({
                .id = CLAY_ID("QueryViewButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = g_currentView == 1 ? COLOR_HIGHLIGHT : COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Query"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
            
            // Settings view button
            CLAY({
                .id = CLAY_ID("SettingsViewButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = g_currentView == 2 ? COLOR_HIGHLIGHT : COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Settings"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
        }
    }
}

// Render the graph view
void RenderGraphView(void) {
    CLAY({
        .id = CLAY_ID("GraphView"),
        .layout = { 
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        }
    }) {
        // If connected, render the graph
        if (g_atomSpaceConnected) {
            // In a real implementation, this would render the actual AtomSpace graph
            // using the OpenCogAtomSpaceBridge_GetVisualizationGraph function
            
            // For now, just display a message
            CLAY({
                .id = CLAY_ID("GraphPlaceholder"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
                    .childGap = 16
                }
            }) {
                CLAY_TEXT(
                    CLAY_STRING("AtomSpace Graph Visualization"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 20, .textColor = COLOR_TEXT })
                );
                
                CLAY_TEXT(
                    CLAY_STRING("Connected to: "), 
                    CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT })
                );
                
                CLAY_TEXT(
                    CLAY_STRING(g_storageURI), 
                    CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_SUCCESS })
                );
                
                // Refresh button
                CLAY({
                    .id = CLAY_ID("RefreshButton"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BUTTON,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Refresh"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
            }
        } else {
            // Display a message to connect
            CLAY({
                .id = CLAY_ID("NotConnectedMessage"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
                    .childGap = 16
                }
            }) {
                CLAY_TEXT(
                    CLAY_STRING("Not connected to an AtomSpace.\nGo to Settings to connect."), 
                    CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_WARNING })
                );
            }
        }
    }
}

// Render the query view
void RenderQueryView(void) {
    CLAY({
        .id = CLAY_ID("QueryView"),
        .layout = { 
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
        // Query input area
        CLAY({
            .id = CLAY_ID("QueryInputArea"),
            .layout = { 
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(200) },
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 8
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Query:"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            
            // In a real implementation, this would be a text input field
            CLAY({
                .id = CLAY_ID("QueryInput"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(8)
                },
                .backgroundColor = COLOR_BACKGROUND,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                // For CogServer, suggest a valid scheme pattern
                if (strcmp(g_storageType, "cogserver") == 0) {
                    CLAY_TEXT(
                        CLAY_STRING("(Get (Concept \"example\"))"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = (Clay_Color){180, 180, 180, 255} })
                    );
                } else {
                    CLAY_TEXT(
                        CLAY_STRING("(Enter your query here)"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = (Clay_Color){180, 180, 180, 255} })
                    );
                }
            }
            
            // Execute button
            CLAY({
                .id = CLAY_ID("ExecuteButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIXED(36) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = g_atomSpaceConnected ? COLOR_BUTTON : (Clay_Color){67, 70, 75, 128},
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Execute"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
        }
        
        // Results area
        CLAY({
            .id = CLAY_ID("QueryResultsArea"),
            .layout = { 
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 8
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Results:"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            
            // Results content
            CLAY({
                .id = CLAY_ID("QueryResults"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_BACKGROUND,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(
                    CLAY_STRING("Query results will appear here"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = (Clay_Color){180, 180, 180, 255} })
                );
            }
        }
    }
}

// Render the settings view
void RenderSettingsView(void) {
    CLAY({
        .id = CLAY_ID("SettingsView"),
        .layout = { 
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
        // Connection settings
        CLAY({
            .id = CLAY_ID("ConnectionSettings"),
            .layout = { 
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(300) },
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Connection Settings"), CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
            
            // Storage type
            CLAY({
                .id = CLAY_ID("StorageTypeSection"),
                .layout = { 
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .childGap = 8,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Storage Type:"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                
                // Storage type selector
                CLAY({
                    .id = CLAY_ID("StorageTypeSelector"),
                    .layout = { 
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .childGap = 8
                    }
                }) {
                    for (int i = 0; i < NUM_STORAGE_OPTIONS; i++) {
                        const StorageOption* option = &STORAGE_OPTIONS[i];
                        bool selected = strcmp(g_storageType, option->name) == 0;
                        
                        CLAY({
                            .id = CLAY_ID("StorageOption"),
                            .layout = { 
                                .sizing = { .width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIXED(36) },
                                .padding = CLAY_PADDING_ALL(8),
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = selected ? COLOR_HIGHLIGHT : COLOR_BUTTON,
                            .cornerRadius = CLAY_CORNER_RADIUS(4)
                        }) {
                            CLAY_TEXT(
                                CLAY_STRING(option->name), 
                                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                            );
                        }
                    }
                }
            }
            
            // Storage description
            CLAY({
                .id = CLAY_ID("StorageDescription"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(8)
                },
                .backgroundColor = COLOR_BACKGROUND,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                // Find the current storage option
                const char* description = "Unknown storage type";
                for (int i = 0; i < NUM_STORAGE_OPTIONS; i++) {
                    if (strcmp(g_storageType, STORAGE_OPTIONS[i].name) == 0) {
                        description = STORAGE_OPTIONS[i].description;
                        break;
                    }
                }
                
                CLAY_TEXT(
                    CLAY_STRING(description), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                );
            }
            
            // URI
            CLAY({
                .id = CLAY_ID("URISection"),
                .layout = { 
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .childGap = 8,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("URI:"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                
                // In a real implementation, this would be a text input field
                CLAY({
                    .id = CLAY_ID("URIInput"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_FIXED(350), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(
                        CLAY_STRING(g_storageURI), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                    );
                }
            }
            
            // Connection status
            CLAY({
                .id = CLAY_ID("ConnectionStatus"),
                .layout = { 
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .childGap = 8,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Status:"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                
                CLAY_TEXT(
                    CLAY_STRING(g_atomSpaceConnected ? "Connected" : "Not Connected"), 
                    CLAY_TEXT_CONFIG({ 
                        .fontSize = 14, 
                        .textColor = g_atomSpaceConnected ? COLOR_SUCCESS : COLOR_WARNING 
                    })
                );
            }
            
            // Connect/Disconnect button
            CLAY({
                .id = CLAY_ID("ConnectButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(150), .height = CLAY_SIZING_FIXED(36) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = g_atomSpaceConnected ? COLOR_ERROR : COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(
                    CLAY_STRING(g_atomSpaceConnected ? "Disconnect" : "Connect"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                );
            }
        }
        
        // CogServer specific settings (only shown when cogserver is selected)
        if (strcmp(g_storageType, "cogserver") == 0) {
            CLAY({
                .id = CLAY_ID("CogServerSettings"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(200) },
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 16
                },
                .backgroundColor = COLOR_PANEL,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("CogServer Settings"), CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
                
                // CogServer description
                CLAY({
                    .id = CLAY_ID("CogServerDescription"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .padding = CLAY_PADDING_ALL(8)
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(
                        CLAY_STRING("CogServer provides a network interface to an AtomSpace.\nUse telnet to connect to it and interact with the command line."), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                    );
                }
                
                // Test connection button
                CLAY({
                    .id = CLAY_ID("TestConnectionButton"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_FIXED(150), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BUTTON,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(
                        CLAY_STRING("Test Connection"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                    );
                }
            }
        }
        
        // Visualization settings
        CLAY({
            .id = CLAY_ID("VisualizationSettings"),
            .layout = { 
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(200) },
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Visualization Settings"), CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
            
            // Layout algorithm
            CLAY({
                .id = CLAY_ID("LayoutAlgorithmSection"),
                .layout = { 
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .childGap = 8,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Layout Algorithm:"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                
                // In a real implementation, this would be a dropdown menu
                CLAY({
                    .id = CLAY_ID("LayoutAlgorithmInput"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(
                        CLAY_STRING("Force-Directed"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                    );
                }
            }
            
            // Node size
            CLAY({
                .id = CLAY_ID("NodeSizeSection"),
                .layout = { 
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .childGap = 8,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Node Size:"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                
                // In a real implementation, this would be a slider
                CLAY({
                    .id = CLAY_ID("NodeSizeInput"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(
                        CLAY_STRING("25"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                    );
                }
            }
        }
    }
}

// Handle UI interactions
void HandleUIInteractions() {
    // Check if the user clicked on a view button
    Clay_ElementInfo* graphViewButton = Clay_GetElementByID(CLAY_ID("GraphViewButton"));
    Clay_ElementInfo* queryViewButton = Clay_GetElementByID(CLAY_ID("QueryViewButton"));
    Clay_ElementInfo* settingsViewButton = Clay_GetElementByID(CLAY_ID("SettingsViewButton"));
    
    if (graphViewButton && graphViewButton->isHovered && g_mousePressed) {
        g_currentView = 0;
    } else if (queryViewButton && queryViewButton->isHovered && g_mousePressed) {
        g_currentView = 1;
    } else if (settingsViewButton && settingsViewButton->isHovered && g_mousePressed) {
        g_currentView = 2;
    }
    
    // Check if the user clicked on the connect/disconnect button
    Clay_ElementInfo* connectButton = Clay_GetElementByID(CLAY_ID("ConnectButton"));
    if (connectButton && connectButton->isHovered && g_mousePressed) {
        if (g_atomSpaceConnected) {
            DisconnectFromAtomSpace();
        } else {
            ConnectToAtomSpace(g_storageType, g_storageURI);
        }
    }
    
    // Check if the user clicked on a storage option
    Clay_ElementInfo* storageOption = Clay_GetElementByID(CLAY_ID("StorageOption"));
    if (storageOption && storageOption->isHovered && g_mousePressed) {
        for (int i = 0; i < NUM_STORAGE_OPTIONS; i++) {
            const StorageOption* option = &STORAGE_OPTIONS[i];
            Clay_BoundingBox bbox = storageOption->boundingBox;
            
            // Adjust bounding box based on the option's position
            bbox.x += i * (120 + 8); // width + gap
            
            if (g_mouseX >= bbox.x && g_mouseX < bbox.x + bbox.width &&
                g_mouseY >= bbox.y && g_mouseY < bbox.y + bbox.height) {
                // Change storage type
                strncpy(g_storageType, option->name, sizeof(g_storageType) - 1);
                
                // Update URI to default for this storage type
                strncpy(g_storageURI, option->default_uri, sizeof(g_storageURI) - 1);
                
                // If connected, disconnect first
                if (g_atomSpaceConnected) {
                    DisconnectFromAtomSpace();
                }
                
                break;
            }
        }
    }
    
    // Check if the user clicked on the refresh button
    Clay_ElementInfo* refreshButton = Clay_GetElementByID(CLAY_ID("RefreshButton"));
    if (refreshButton && refreshButton->isHovered && g_mousePressed && g_atomSpaceConnected) {
        // Update visualization if connected
        OpenCogAtomSpaceBridge_UpdateVisualization();
    }
    
    // Check if the user clicked on the test connection button (CogServer specific)
    Clay_ElementInfo* testConnectionButton = Clay_GetElementByID(CLAY_ID("TestConnectionButton"));
    if (testConnectionButton && testConnectionButton->isHovered && g_mousePressed) {
        // This would test the connection without fully connecting
        printf("Testing connection to CogServer at %s\n", g_storageURI);
    }
}

// Main entry point
int main(int argc, char** argv) {
    // Initialize the renderer
    if (!OpenCogRenderer_Initialize(1024, 768, "AtomSpace Viewer")) {
        fprintf(stderr, "Failed to initialize renderer\n");
        return 1;
    }
    
    // Set up Clay
    Clay_Initialize(OpenCogRenderer_MeasureText, 0);
    Clay_SetErrorHandler(HandleClayErrors);
    
    // Main loop
    while (OpenCogRenderer_Update()) {
        // Update Clay dimensions in case window was resized
        Clay_SetLayoutDimensions((Clay_Dimensions){ OpenCogRenderer_GetWidth(), OpenCogRenderer_GetHeight() });
        
        // Update mouse state
        Clay_SetPointerState((Clay_Vector2){ g_mouseX, g_mouseY }, g_mousePressed);
        
        // Update scroll containers
        Clay_UpdateScrollContainers(true, (Clay_Vector2){ 0, g_mouseWheel * 10 }, 0.016f);
        
        // Handle UI interactions
        HandleUIInteractions();
        
        // Begin layout
        Clay_BeginLayout();
        
        // Main container
        CLAY({ 
            .id = CLAY_ID("MainContainer"), 
            .layout = { 
                .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                .childGap = 0
            }, 
            .backgroundColor = COLOR_BACKGROUND
        }) {
            // Toolbar
            RenderToolbar();
            
            // Content area
            if (g_currentView == 0) {
                RenderGraphView();
            } else if (g_currentView == 1) {
                RenderQueryView();
            } else {
                RenderSettingsView();
            }
        }
        
        // End layout and render
        Clay_RenderCommandArray commands = Clay_EndLayout();
        OpenCogRenderer_Render(commands);
    }
    
    // Shutdown
    if (g_atomSpaceConnected) {
        OpenCogAtomSpaceBridge_Shutdown();
    }
    
    Clay_Shutdown();
    OpenCogRenderer_Shutdown();
    
    return 0;
} 