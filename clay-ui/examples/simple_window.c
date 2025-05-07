#define CLAY_IMPLEMENTATION
#include "../src/clay.h"
#include "../include/opencog_renderer.h"
#include <stdio.h>
#include <stdlib.h>

// Define some colors
const Clay_Color COLOR_LIGHT = (Clay_Color) {224, 215, 210, 255};
const Clay_Color COLOR_RED = (Clay_Color) {168, 66, 28, 255};
const Clay_Color COLOR_ORANGE = (Clay_Color) {225, 138, 50, 255};
const Clay_Color COLOR_BLUE = (Clay_Color) {60, 120, 230, 255};
const Clay_Color COLOR_DARK = (Clay_Color) {30, 30, 30, 255};
const Clay_Color COLOR_WHITE = (Clay_Color) {250, 250, 250, 255};

// Error handler for Clay
void HandleClayErrors(Clay_ErrorData errorData) {
    printf("Clay Error: %.*s\n", errorData.errorText.length, errorData.errorText.chars);
    switch(errorData.errorType) {
        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_FUNCTION_NOT_PROVIDED:
            printf("Text measurement function not provided\n");
            break;
        case CLAY_ERROR_TYPE_ARENA_CAPACITY_EXCEEDED:
            printf("Arena capacity exceeded\n");
            break;
        case CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED:
            printf("Elements capacity exceeded\n");
            break;
        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED:
            printf("Text measurement capacity exceeded\n");
            break;
        case CLAY_ERROR_TYPE_DUPLICATE_ID:
            printf("Duplicate ID\n");
            break;
        case CLAY_ERROR_TYPE_FLOATING_CONTAINER_PARENT_NOT_FOUND:
            printf("Floating container parent not found\n");
            break;
        case CLAY_ERROR_TYPE_INTERNAL_ERROR:
            printf("Internal error\n");
            break;
        default:
            printf("Unknown error\n");
            break;
    }
}

// Re-usable component for sidebar items
void SidebarItemComponent(Clay_String text) {
    CLAY({
        .layout = {
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = COLOR_ORANGE,
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_WHITE }));
    }
}

int main() {
    // Initialize the renderer
    if (!OpenCogRenderer_Initialize(1024, 768, "Clay UI for OpenCog - Simple Example")) {
        printf("Failed to initialize renderer\n");
        return 1;
    }

    // Get memory size needed for Clay
    uint64_t totalMemorySize = Clay_MinMemorySize();
    printf("Clay memory size: %llu bytes\n", (unsigned long long)totalMemorySize);

    // Allocate memory for Clay
    void* clayMemory = malloc(totalMemorySize);
    if (!clayMemory) {
        printf("Failed to allocate memory for Clay\n");
        OpenCogRenderer_Shutdown();
        return 1;
    }

    // Create Clay arena
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, clayMemory);

    // Initialize Clay
    Clay_Initialize(arena, 
                    (Clay_Dimensions){ OpenCogRenderer_GetWidth(), OpenCogRenderer_GetHeight() }, 
                    (Clay_ErrorHandler){ HandleClayErrors });

    // Set the text measurement function
    Clay_SetMeasureTextFunction(OpenCogRenderer_MeasureText, 0);

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
                .padding = CLAY_PADDING_ALL(16), 
                .childGap = 16 
            }, 
            .backgroundColor = COLOR_DARK 
        }) {
            // Header
            CLAY({
                .id = CLAY_ID("Header"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0) }, 
                    .padding = CLAY_PADDING_ALL(16), 
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER } 
                },
                .backgroundColor = COLOR_BLUE,
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                CLAY_TEXT(CLAY_STRING("OpenCog with Clay UI"), CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = COLOR_WHITE }));
            }

            // Content area with sidebar and main panel
            CLAY({
                .id = CLAY_ID("ContentArea"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                    .childGap = 16
                }
            }) {
                // Sidebar
                CLAY({
                    .id = CLAY_ID("Sidebar"),
                    .layout = { 
                        .layoutDirection = CLAY_TOP_TO_BOTTOM, 
                        .sizing = { .width = CLAY_SIZING_FIXED(250), .height = CLAY_SIZING_GROW(0) }, 
                        .padding = CLAY_PADDING_ALL(16), 
                        .childGap = 8 
                    },
                    .backgroundColor = COLOR_LIGHT,
                    .cornerRadius = CLAY_CORNER_RADIUS(8)
                }) {
                    // Sidebar title
                    CLAY_TEXT(CLAY_STRING("Navigation"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_DARK }));
                    
                    // Sidebar items
                    SidebarItemComponent(CLAY_STRING("AtomSpace Viewer"));
                    SidebarItemComponent(CLAY_STRING("Pattern Matcher"));
                    SidebarItemComponent(CLAY_STRING("Rule Engine"));
                    SidebarItemComponent(CLAY_STRING("PLN Inference"));
                    SidebarItemComponent(CLAY_STRING("Language Learning"));
                    SidebarItemComponent(CLAY_STRING("Settings"));
                }

                // Main content area
                CLAY({
                    .id = CLAY_ID("MainContent"),
                    .layout = { 
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }, 
                        .padding = CLAY_PADDING_ALL(16), 
                        .childGap = 16 
                    },
                    .backgroundColor = COLOR_LIGHT,
                    .cornerRadius = CLAY_CORNER_RADIUS(8)
                }) {
                    // Title
                    CLAY_TEXT(CLAY_STRING("AtomSpace Viewer"), CLAY_TEXT_CONFIG({ .fontSize = 20, .textColor = COLOR_DARK }));
                    
                    // Content
                    CLAY({
                        .id = CLAY_ID("GraphView"),
                        .layout = { 
                            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(300) }
                        },
                        .backgroundColor = COLOR_WHITE,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        CLAY_TEXT(
                            CLAY_STRING("This area would display an interactive graph of Atoms.\nNot implemented in this example."), 
                            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_DARK })
                        );
                    }
                    
                    // Node details
                    CLAY({
                        .id = CLAY_ID("NodeDetails"),
                        .layout = { 
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                            .padding = CLAY_PADDING_ALL(16),
                            .childGap = 8
                        },
                        .backgroundColor = COLOR_WHITE,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        CLAY_TEXT(CLAY_STRING("Node Details"), CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_DARK }));
                        
                        // Properties of the selected node
                        CLAY({
                            .layout = { 
                                .childGap = 8
                            }
                        }) {
                            CLAY_TEXT(CLAY_STRING("Type: ConceptNode"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_DARK }));
                            CLAY_TEXT(CLAY_STRING("Name: \"example\""), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_DARK }));
                            CLAY_TEXT(CLAY_STRING("TruthValue: (0.8, 0.5)"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_DARK }));
                            CLAY_TEXT(CLAY_STRING("Incoming: 3 links"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_DARK }));
                            CLAY_TEXT(CLAY_STRING("Outgoing: 0 links"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_DARK }));
                        }
                    }
                }
            }
        }

        // End layout and get render commands
        Clay_RenderCommandArray renderCommands = Clay_EndLayout();

        // Render the UI
        OpenCogRenderer_Render(renderCommands);
    }

    // Cleanup
    free(clayMemory);
    OpenCogRenderer_Shutdown();

    return 0;
} 