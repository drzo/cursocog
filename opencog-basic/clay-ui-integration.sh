#!/bin/bash
#
# OpenCog Basic - Clay UI Integration
# This script integrates Clay UI into the OpenCog Basic repository for visualization
#

set -e  # Exit on error

echo "OpenCog Basic - Clay UI Integration"
echo "=================================="
echo

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Function to display error and exit
function error_exit {
    echo -e "${RED}$1${NC}" 1>&2
    exit 1
}

# Check if clay-ui exists in parent directory
if [ ! -d "../clay-ui" ]; then
    error_exit "Clay UI directory not found in parent directory."
fi

# Create clay-ui subdirectory in opencog-basic
CLAY_UI_DIR="clay-ui"
mkdir -p $CLAY_UI_DIR

# Copy clay-ui files to opencog-basic
echo -e "${YELLOW}Copying Clay UI files to OpenCog Basic...${NC}"

# Create subdirectories
for dir in src include examples cmake; do
    mkdir -p "$CLAY_UI_DIR/$dir"
done

# Copy essential files
cp -r ../clay-ui/CMakeLists.txt $CLAY_UI_DIR/
cp -r ../clay-ui/src/* $CLAY_UI_DIR/src/ 2>/dev/null || true
cp -r ../clay-ui/include/* $CLAY_UI_DIR/include/ 2>/dev/null || true
cp -r ../clay-ui/examples/* $CLAY_UI_DIR/examples/ 2>/dev/null || true
cp -r ../clay-ui/cmake/* $CLAY_UI_DIR/cmake/ 2>/dev/null || true
cp -r ../clay-ui/download.sh $CLAY_UI_DIR/ 2>/dev/null || true
chmod +x $CLAY_UI_DIR/download.sh 2>/dev/null || true

# Create a launcher and dashboard application
echo -e "${YELLOW}Creating OpenCog Dashboard application...${NC}"

DASHBOARD_DIR="$CLAY_UI_DIR/examples/dashboard"
mkdir -p $DASHBOARD_DIR

# Create dashboard.cpp - a user-friendly interface for beginners
cat > $DASHBOARD_DIR/dashboard.cpp << 'EOF'
#define CLAY_IMPLEMENTATION
#include "../include/clay.h"
#include "../include/opencog_renderer.h"
#include "../include/opencog_atomspace_bridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

// Global states
static float g_mouseX = 0;
static float g_mouseY = 0;
static bool g_mousePressed = false;
static float g_mouseWheel = 0;
static int g_currentView = 0; // 0 = welcome, 1 = installation, 2 = tutorials, 3 = atomspace viewer

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

// Render the toolbar with navigation
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
        CLAY_TEXT(CLAY_STRING("OpenCog Dashboard"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT }));
        
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
            // Welcome button
            CLAY({
                .id = CLAY_ID("WelcomeButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = g_currentView == 0 ? COLOR_HIGHLIGHT : COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Welcome"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
            
            // Installation button
            CLAY({
                .id = CLAY_ID("InstallationButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = g_currentView == 1 ? COLOR_HIGHLIGHT : COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Install"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
            
            // Tutorials button
            CLAY({
                .id = CLAY_ID("TutorialsButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = g_currentView == 2 ? COLOR_HIGHLIGHT : COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Tutorials"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
            
            // AtomSpace Viewer button
            CLAY({
                .id = CLAY_ID("AtomSpaceButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIXED(34) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = g_currentView == 3 ? COLOR_HIGHLIGHT : COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("AtomSpace"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
        }
    }
}

// Render the welcome view
void RenderWelcomeView(void) {
    CLAY({
        .id = CLAY_ID("WelcomeView"),
        .layout = { 
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
            .childGap = 20
        }
    }) {
        CLAY_TEXT(
            CLAY_STRING("Welcome to OpenCog"), 
            CLAY_TEXT_CONFIG({ .fontSize = 32, .textColor = COLOR_TEXT })
        );
        
        CLAY_TEXT(
            CLAY_STRING("A framework for artificial general intelligence"), 
            CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT })
        );
        
        CLAY({
            .id = CLAY_ID("GetStartedButton"),
            .layout = { 
                .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_FIXED(50) },
                .padding = CLAY_PADDING_ALL(8),
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = COLOR_HIGHLIGHT,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Get Started →"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT }));
        }
        
        CLAY_TEXT(
            CLAY_STRING("Version: OpenCog Basic 0.1"), 
            CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = (Clay_Color){180, 180, 180, 255} })
        );
    }
}

// Render the installation view
void RenderInstallationView(void) {
    CLAY({
        .id = CLAY_ID("InstallationView"),
        .layout = { 
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(20),
            .childGap = 20
        }
    }) {
        CLAY_TEXT(
            CLAY_STRING("Installation & Setup"), 
            CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = COLOR_TEXT })
        );
        
        // Dependencies panel
        CLAY({
            .id = CLAY_ID("DependenciesPanel"),
            .layout = { 
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(300) },
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Dependencies"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT }));
            
            CLAY_TEXT(
                CLAY_STRING("The following dependencies are required to build OpenCog:"), 
                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
            );
            
            // List of dependencies
            CLAY({
                .id = CLAY_ID("DependenciesList"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(10),
                    .childGap = 8
                },
                .backgroundColor = COLOR_BACKGROUND,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(
                    CLAY_STRING("• C++17 compiler (GCC 7.5+, Clang 6+, MSVC 2019+)\n• CMake 3.12+\n• Boost 1.68+\n• Python 3.6+ with development headers"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                );
            }
            
            // Install button
            CLAY({
                .id = CLAY_ID("InstallDepsButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_FIXED(40) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Install Dependencies"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
        }
        
        // Build panel
        CLAY({
            .id = CLAY_ID("BuildPanel"),
            .layout = { 
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(250) },
                .padding = CLAY_PADDING_ALL(16),
                .childGap = 16
            },
            .backgroundColor = COLOR_PANEL,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Build OpenCog"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT }));
            
            CLAY_TEXT(
                CLAY_STRING("After installing dependencies, you can build OpenCog:"), 
                CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
            );
            
            // Build steps
            CLAY({
                .id = CLAY_ID("BuildSteps"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(10),
                    .childGap = 8
                },
                .backgroundColor = COLOR_BACKGROUND,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(
                    CLAY_STRING("1. Run the build script\n2. Wait for compilation to complete\n3. Start using OpenCog components"), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                );
            }
            
            // Build button
            CLAY({
                .id = CLAY_ID("BuildButton"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_FIXED(40) },
                    .padding = CLAY_PADDING_ALL(8),
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_BUTTON,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Build OpenCog"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
            }
        }
    }
}

// Render the tutorials view
void RenderTutorialsView(void) {
    CLAY({
        .id = CLAY_ID("TutorialsView"),
        .layout = { 
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(20),
            .childGap = 20
        }
    }) {
        CLAY_TEXT(
            CLAY_STRING("Tutorials & Learning Resources"), 
            CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = COLOR_TEXT })
        );
        
        // Tutorial cards in a grid layout
        CLAY({
            .id = CLAY_ID("TutorialGrid"),
            .layout = { 
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                .childGap = 16,
                .flexWrap = true
            }
        }) {
            // Tutorial card 1
            CLAY({
                .id = CLAY_ID("TutorialCard1"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_FIXED(200) },
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 12
                },
                .backgroundColor = COLOR_PANEL,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Getting Started"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT }));
                
                CLAY_TEXT(
                    CLAY_STRING("Learn the basics of OpenCog and how to use the AtomSpace."), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                );
                
                CLAY({
                    .id = CLAY_ID("OpenTutorial1"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BUTTON,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Open Tutorial"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
            }
            
            // Tutorial card 2
            CLAY({
                .id = CLAY_ID("TutorialCard2"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_FIXED(200) },
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 12
                },
                .backgroundColor = COLOR_PANEL,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Pattern Matching"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT }));
                
                CLAY_TEXT(
                    CLAY_STRING("Understand how to use the pattern matcher to query the AtomSpace."), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                );
                
                CLAY({
                    .id = CLAY_ID("OpenTutorial2"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BUTTON,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Open Tutorial"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
            }
            
            // Tutorial card 3
            CLAY({
                .id = CLAY_ID("TutorialCard3"),
                .layout = { 
                    .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_FIXED(200) },
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 12
                },
                .backgroundColor = COLOR_PANEL,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                CLAY_TEXT(CLAY_STRING("Persistence"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = COLOR_TEXT }));
                
                CLAY_TEXT(
                    CLAY_STRING("Learn how to persist AtomSpace data using different storage backends."), 
                    CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT })
                );
                
                CLAY({
                    .id = CLAY_ID("OpenTutorial3"),
                    .layout = { 
                        .sizing = { .width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIXED(36) },
                        .padding = CLAY_PADDING_ALL(8),
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = COLOR_BUTTON,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    CLAY_TEXT(CLAY_STRING("Open Tutorial"), CLAY_TEXT_CONFIG({ .fontSize = 14, .textColor = COLOR_TEXT }));
                }
            }
        }
    }
}

// Placeholder for the AtomSpace viewer
void RenderAtomSpaceView(void) {
    CLAY({
        .id = CLAY_ID("AtomSpaceView"),
        .layout = { 
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
            .childGap = 20
        }
    }) {
        CLAY_TEXT(
            CLAY_STRING("AtomSpace Viewer"), 
            CLAY_TEXT_CONFIG({ .fontSize = 24, .textColor = COLOR_TEXT })
        );
        
        CLAY_TEXT(
            CLAY_STRING("Launch the full AtomSpace Viewer to visualize and interact with the AtomSpace."), 
            CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT })
        );
        
        CLAY({
            .id = CLAY_ID("LaunchViewerButton"),
            .layout = { 
                .sizing = { .width = CLAY_SIZING_FIXED(250), .height = CLAY_SIZING_FIXED(50) },
                .padding = CLAY_PADDING_ALL(8),
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = COLOR_HIGHLIGHT,
            .cornerRadius = CLAY_CORNER_RADIUS(4)
        }) {
            CLAY_TEXT(CLAY_STRING("Launch AtomSpace Viewer"), CLAY_TEXT_CONFIG({ .fontSize = 16, .textColor = COLOR_TEXT }));
        }
    }
}

// Handle UI interactions
void HandleUIInteractions() {
    // Check if the user clicked on a navigation button
    Clay_ElementInfo* welcomeButton = Clay_GetElementByID(CLAY_ID("WelcomeButton"));
    Clay_ElementInfo* installationButton = Clay_GetElementByID(CLAY_ID("InstallationButton"));
    Clay_ElementInfo* tutorialsButton = Clay_GetElementByID(CLAY_ID("TutorialsButton"));
    Clay_ElementInfo* atomSpaceButton = Clay_GetElementByID(CLAY_ID("AtomSpaceButton"));
    
    if (welcomeButton && welcomeButton->isHovered && g_mousePressed) {
        g_currentView = 0;
    } else if (installationButton && installationButton->isHovered && g_mousePressed) {
        g_currentView = 1;
    } else if (tutorialsButton && tutorialsButton->isHovered && g_mousePressed) {
        g_currentView = 2;
    } else if (atomSpaceButton && atomSpaceButton->isHovered && g_mousePressed) {
        g_currentView = 3;
    }
    
    // Check if the user clicked on "Get Started" button
    Clay_ElementInfo* getStartedButton = Clay_GetElementByID(CLAY_ID("GetStartedButton"));
    if (getStartedButton && getStartedButton->isHovered && g_mousePressed) {
        g_currentView = 1; // Go to installation view
    }
    
    // Check if the user clicked on the launch viewer button
    Clay_ElementInfo* launchViewerButton = Clay_GetElementByID(CLAY_ID("LaunchViewerButton"));
    if (launchViewerButton && launchViewerButton->isHovered && g_mousePressed) {
        printf("Launching AtomSpace Viewer...\n");
        // In a real implementation, this would launch the AtomSpace Viewer application
    }
    
    // Check if the user clicked on the install dependencies button
    Clay_ElementInfo* installDepsButton = Clay_GetElementByID(CLAY_ID("InstallDepsButton"));
    if (installDepsButton && installDepsButton->isHovered && g_mousePressed) {
        printf("Installing dependencies...\n");
        // In a real implementation, this would trigger the dependency installer
    }
    
    // Check if the user clicked on the build button
    Clay_ElementInfo* buildButton = Clay_GetElementByID(CLAY_ID("BuildButton"));
    if (buildButton && buildButton->isHovered && g_mousePressed) {
        printf("Building OpenCog...\n");
        // In a real implementation, this would trigger the build process
    }
}

// Main entry point
int main(int argc, char** argv) {
    // Initialize the renderer
    if (!OpenCogRenderer_Initialize(1024, 768, "OpenCog Dashboard")) {
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
                RenderWelcomeView();
            } else if (g_currentView == 1) {
                RenderInstallationView();
            } else if (g_currentView == 2) {
                RenderTutorialsView();
            } else {
                RenderAtomSpaceView();
            }
        }
        
        // End layout and render
        Clay_RenderCommandArray commands = Clay_EndLayout();
        OpenCogRenderer_Render(commands);
    }
    
    // Shutdown
    Clay_Shutdown();
    OpenCogRenderer_Shutdown();
    
    return 0;
}
EOF

# Create installer script
cat > $CLAY_UI_DIR/installer.sh << 'EOF'
#!/bin/bash

echo "OpenCog Basic - Dependency Installer"
echo "==================================="
echo

echo "Checking for required tools..."
command -v cmake >/dev/null 2>&1 || { 
    echo "ERROR: CMake not found. Please install CMake."
    echo "You can install CMake with: sudo apt-get install cmake"
    exit 1
}

command -v git >/dev/null 2>&1 || {
    echo "ERROR: Git not found. Please install Git."
    echo "You can install Git with: sudo apt-get install git"
    exit 1
}

echo "Installing dependencies..."
./install-dependencies.sh

echo
echo "Dependencies installed successfully!"
echo "You can now build OpenCog Basic by running:"
echo "    ./build.sh"
echo
EOF

chmod +x $CLAY_UI_DIR/installer.sh

# Create a launcher for the dashboard
cat > opencog-dashboard.sh << 'EOF'
#!/bin/bash

echo "Launching OpenCog Dashboard..."
echo

if [ ! -f "clay-ui/build/dashboard" ]; then
    echo "Building dashboard..."
    cd clay-ui
    mkdir -p build
    cd build
    cmake ..
    cmake --build . --target dashboard
    cd ../..
fi

./clay-ui/build/dashboard
EOF

chmod +x opencog-dashboard.sh

# Update the main CMakeLists.txt to include clay-ui
echo -e "${YELLOW}Updating CMakeLists.txt to include Clay UI...${NC}"

cat >> CMakeLists.txt << 'EOF'
# Include Clay UI
add_subdirectory(clay-ui)
EOF

# Update README.md to include Clay UI information
echo -e "${YELLOW}Updating README.md to include Clay UI information...${NC}"

cat >> README.md << 'EOF'

## Graphical User Interface

OpenCog Basic includes Clay UI, a visualization and interaction system for OpenCog components:

### OpenCog Dashboard

The Dashboard provides a user-friendly interface for:
- Installing dependencies
- Building components
- Accessing tutorials
- Visualizing the AtomSpace

To launch the dashboard:

```bash
# On Linux
./opencog-dashboard.sh

# On Windows
opencog-dashboard.bat
```

### AtomSpace Viewer

A dedicated tool for visualizing and interacting with the AtomSpace:

```bash
# Build and run
cd clay-ui/build
cmake ..
cmake --build . --target atomspace-viewer
./atomspace-viewer
```
EOF

echo
echo -e "${GREEN}Clay UI integration complete!${NC}"
echo "You can now use the Clay UI visualization system with OpenCog Basic."
echo
echo -e "${YELLOW}Recommended next steps:${NC}"
echo "1. Review the updated README.md"
echo "2. Run ./opencog-dashboard.sh to launch the dashboard"
echo "3. Build the Clay UI components with the build script" 