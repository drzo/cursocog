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