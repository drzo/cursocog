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
static bool g_atomSpaceConnected = false;
static const char* g_selectedAtomId = nullptr;
static int g_currentView = 0; // 0 = graph, 1 = query, 2 = settings

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
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                CLAY_TEXT(
                    CLAY_STRING("AtomSpace Graph Visualization\n(Placeholder for actual graph rendering)"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT })
                );
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
                CLAY_TEXT(
                    CLAY_STRING("(Enter your query here)"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = (Clay_Color){180, 180, 180, 255} })
                );
            }
            
            // Execute button
            CLAY({
                .id = CLAY_ID("ExecuteButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIXED(36) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_BUTTON,
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
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(250) },
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
                
                // In a real implementation, this would be a dropdown menu
                CLAY({
                    .id = CLAY_ID("StorageTypeInput"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(
                        CLAY_STRING("rocks"), 
                        CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                    );
                }
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
                        .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BACKGROUND,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(
                        CLAY_STRING("rocks:///tmp/atomspace-rocks"), 
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
    
    // Create a demo AtomSpace for testing
    // In a real implementation, this would be replaced with an actual AtomSpace connection
    ConnectToAtomSpace("rocks", "rocks:///tmp/atomspace-rocks");
    
    // Main loop
    while (OpenCogRenderer_Update()) {
        // Update Clay dimensions in case window was resized
        Clay_SetLayoutDimensions((Clay_Dimensions){ OpenCogRenderer_GetWidth(), OpenCogRenderer_GetHeight() });
        
        // Update mouse state
        Clay_SetPointerState((Clay_Vector2){ g_mouseX, g_mouseY }, g_mousePressed);
        
        // Update scroll containers
        Clay_UpdateScrollContainers(true, (Clay_Vector2){ 0, g_mouseWheel * 10 }, 0.016f);
        
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