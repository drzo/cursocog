#include "../include/opencog_renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

// OpenGL includes
#include <GL/glew.h>

#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glx.h>
#endif

// Global variables for window handling
static uint32_t g_width = 800;
static uint32_t g_height = 600;
static bool g_isRunning = false;

#ifdef _WIN32
static HWND g_window = NULL;
static HDC g_deviceContext = NULL;
static HGLRC g_renderContext = NULL;
#else
static Display* g_display = NULL;
static Window g_window = 0;
static GLXContext g_context = NULL;
#endif

// Mouse state
static bool g_mousePressed = false;
static int g_mouseX = 0;
static int g_mouseY = 0;
static int g_mouseWheel = 0;

// Colors
static const Clay_Color COLOR_BACKGROUND = { 30, 30, 30, 255 };
static const Clay_Color COLOR_TEXT = { 230, 230, 230, 255 };
static const Clay_Color COLOR_ACCENT = { 100, 149, 237, 255 }; // Cornflower blue

// Fonts
enum {
    FONT_DEFAULT = 0,
    FONT_MONO = 1,
};

// Simple font metrics for basic text measurement
typedef struct {
    float charWidth;
    float charHeight;
} FontMetrics;

static FontMetrics g_fontMetrics[2] = {
    { 8.0f, 16.0f },   // Default font
    { 8.0f, 16.0f }    // Mono font
};

// OpenGL texture for simple font rendering
static GLuint g_fontTexture = 0;

// Initialize the OpenCog renderer
bool OpenCogRenderer_Initialize(uint32_t width, uint32_t height, const char* title) {
    g_width = width;
    g_height = height;
    
    // Create a window using the platform-specific API
#ifdef _WIN32
    // Windows implementation
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_OWNDC,
        WndProc,
        0,
        0,
        GetModuleHandle(NULL),
        NULL,
        LoadCursor(NULL, IDC_ARROW),
        NULL,
        NULL,
        L"ClayOpenCogRenderer",
        NULL
    };
    
    RegisterClassEx(&wc);
    
    RECT windowRect = { 0, 0, width, height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    
    // Convert title to wide string
    int titleLen = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
    wchar_t* wideTitle = (wchar_t*)malloc(titleLen * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wideTitle, titleLen);
    
    g_window = CreateWindow(
        L"ClayOpenCogRenderer",
        wideTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );
    
    free(wideTitle);
    
    if (!g_window) {
        printf("Failed to create window\n");
        return false;
    }
    
    g_deviceContext = GetDC(g_window);
    
    // Setup OpenGL context
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32, // Color bits
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24, // Depth bits
        8,  // Stencil bits
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    
    int pixelFormat = ChoosePixelFormat(g_deviceContext, &pfd);
    SetPixelFormat(g_deviceContext, pixelFormat, &pfd);
    
    g_renderContext = wglCreateContext(g_deviceContext);
    wglMakeCurrent(g_deviceContext, g_renderContext);
    
    ShowWindow(g_window, SW_SHOW);
#else
    // X11 implementation
    g_display = XOpenDisplay(NULL);
    if (!g_display) {
        printf("Failed to open X display\n");
        return false;
    }
    
    int screen = DefaultScreen(g_display);
    
    // Choose visual with OpenGL support
    static int attributes[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        None
    };
    
    XVisualInfo* visualInfo = glXChooseVisual(g_display, screen, attributes);
    if (!visualInfo) {
        printf("Failed to get a suitable visual\n");
        XCloseDisplay(g_display);
        return false;
    }
    
    // Create window
    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap = XCreateColormap(g_display, RootWindow(g_display, screen), visualInfo->visual, AllocNone);
    windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    
    g_window = XCreateWindow(
        g_display,
        RootWindow(g_display, screen),
        0, 0,   // Position
        width, height, // Size
        0,      // Border width
        visualInfo->depth,
        InputOutput,
        visualInfo->visual,
        CWColormap | CWEventMask,
        &windowAttributes
    );
    
    // Set window title
    XStoreName(g_display, g_window, title);
    
    // Create GLX context
    g_context = glXCreateContext(g_display, visualInfo, NULL, GL_TRUE);
    glXMakeCurrent(g_display, g_window, g_context);
    
    // Show window
    XMapWindow(g_display, g_window);
    XFlush(g_display);
    
    XFree(visualInfo);
#endif
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        printf("GLEW initialization failed: %s\n", glewGetErrorString(glewErr));
        OpenCogRenderer_Shutdown();
        return false;
    }
    
    // Set OpenGL viewport
    glViewport(0, 0, width, height);
    
    // Initialize OpenGL settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // TODO: Initialize font texture here
    
    g_isRunning = true;
    return true;
}

// Shutdown the renderer
void OpenCogRenderer_Shutdown() {
    // Delete OpenGL resources
    if (g_fontTexture != 0) {
        glDeleteTextures(1, &g_fontTexture);
    }
    
#ifdef _WIN32
    // Windows cleanup
    if (g_renderContext) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(g_renderContext);
    }
    
    if (g_window) {
        ReleaseDC(g_window, g_deviceContext);
        DestroyWindow(g_window);
    }
    
    UnregisterClass(L"ClayOpenCogRenderer", GetModuleHandle(NULL));
#else
    // X11 cleanup
    if (g_context) {
        glXMakeCurrent(g_display, None, NULL);
        glXDestroyContext(g_display, g_context);
    }
    
    if (g_window) {
        XDestroyWindow(g_display, g_window);
    }
    
    if (g_display) {
        XCloseDisplay(g_display);
    }
#endif
    
    g_isRunning = false;
}

// Render a rectangle
static void RenderRectangle(Clay_BoundingBox boundingBox, Clay_Color color, Clay_CornerRadius cornerRadius) {
    // Simple flat-colored rectangle
    glBegin(GL_QUADS);
    glColor4f(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    glVertex2f(boundingBox.x, boundingBox.y);
    glVertex2f(boundingBox.x + boundingBox.width, boundingBox.y);
    glVertex2f(boundingBox.x + boundingBox.width, boundingBox.y + boundingBox.height);
    glVertex2f(boundingBox.x, boundingBox.y + boundingBox.height);
    glEnd();
    
    // TODO: Implement rounded corners properly
}

// Render text (simple version)
static void RenderText(Clay_BoundingBox boundingBox, Clay_TextRenderData* textData) {
    // This is a placeholder. In a real implementation, you would use a font renderer.
    float x = boundingBox.x;
    float y = boundingBox.y;
    
    glColor4f(
        textData->textColor.r / 255.0f,
        textData->textColor.g / 255.0f,
        textData->textColor.b / 255.0f,
        textData->textColor.a / 255.0f
    );
    
    // Print the text to console for debugging
    char* tempText = malloc(textData->stringContents.length + 1);
    memcpy(tempText, textData->stringContents.chars, textData->stringContents.length);
    tempText[textData->stringContents.length] = '\0';
    
    printf("Text at (%f, %f): %s\n", x, y, tempText);
    free(tempText);
    
    // TODO: Implement proper text rendering
}

// Render the Clay UI commands
void OpenCogRenderer_Render(Clay_RenderCommandArray commands) {
    // Clear the screen
    glClearColor(
        COLOR_BACKGROUND.r / 255.0f,
        COLOR_BACKGROUND.g / 255.0f,
        COLOR_BACKGROUND.b / 255.0f,
        COLOR_BACKGROUND.a / 255.0f
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, g_width, g_height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Process Clay render commands
    for (uint32_t i = 0; i < commands.length; i++) {
        Clay_RenderCommand* cmd = &commands.internalArray[i];
        
        switch (cmd->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
                RenderRectangle(
                    cmd->boundingBox,
                    cmd->renderData.rectangle.backgroundColor,
                    cmd->renderData.rectangle.cornerRadius
                );
                break;
                
            case CLAY_RENDER_COMMAND_TYPE_TEXT:
                RenderText(cmd->boundingBox, &cmd->renderData.text);
                break;
                
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
                glEnable(GL_SCISSOR_TEST);
                glScissor(
                    (GLint)cmd->boundingBox.x,
                    (GLint)(g_height - cmd->boundingBox.y - cmd->boundingBox.height),
                    (GLsizei)cmd->boundingBox.width,
                    (GLsizei)cmd->boundingBox.height
                );
                break;
                
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
                glDisable(GL_SCISSOR_TEST);
                break;
                
            // TODO: Handle other command types
            
            default:
                break;
        }
    }
    
    // Swap buffers
#ifdef _WIN32
    SwapBuffers(g_deviceContext);
#else
    glXSwapBuffers(g_display, g_window);
#endif
}

// Process window events and update UI state
bool OpenCogRenderer_Update() {
    if (!g_isRunning) {
        return false;
    }
    
    g_mouseWheel = 0; // Reset mouse wheel delta
    
#ifdef _WIN32
    // Windows message processing
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        if (msg.message == WM_QUIT) {
            g_isRunning = false;
            return false;
        }
    }
#else
    // X11 event processing
    XEvent event;
    while (XPending(g_display) > 0) {
        XNextEvent(g_display, &event);
        
        switch (event.type) {
            case Expose:
                // Window needs to be redrawn
                break;
                
            case KeyPress:
                // Handle key press
                // Check for escape key to exit
                if (XLookupKeysym(&event.xkey, 0) == XK_Escape) {
                    g_isRunning = false;
                    return false;
                }
                break;
                
            case ButtonPress:
                // Handle mouse button press
                if (event.xbutton.button == Button1) {
                    g_mousePressed = true;
                } else if (event.xbutton.button == Button4) {
                    // Mouse wheel up
                    g_mouseWheel = 1;
                } else if (event.xbutton.button == Button5) {
                    // Mouse wheel down
                    g_mouseWheel = -1;
                }
                break;
                
            case ButtonRelease:
                // Handle mouse button release
                if (event.xbutton.button == Button1) {
                    g_mousePressed = false;
                }
                break;
                
            case MotionNotify:
                // Handle mouse motion
                g_mouseX = event.xmotion.x;
                g_mouseY = event.xmotion.y;
                break;
                
            case ClientMessage:
                // Check for window close
                break;
        }
    }
#endif
    
    return g_isRunning;
}

// Get the screen width
uint32_t OpenCogRenderer_GetWidth() {
    return g_width;
}

// Get the screen height
uint32_t OpenCogRenderer_GetHeight() {
    return g_height;
}

// Measure text dimensions
Clay_Dimensions OpenCogRenderer_MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, uintptr_t userData) {
    // Simple text measurement using font metrics
    uint32_t fontId = config->fontId < 2 ? config->fontId : FONT_DEFAULT;
    float width = text.length * g_fontMetrics[fontId].charWidth * (config->fontSize / 16.0f);
    float height = g_fontMetrics[fontId].charHeight * (config->fontSize / 16.0f);
    
    return (Clay_Dimensions){width, height};
}

#ifdef _WIN32
// Windows message processing function
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            g_isRunning = false;
            PostQuitMessage(0);
            return 0;
            
        case WM_SIZE:
            g_width = LOWORD(lParam);
            g_height = HIWORD(lParam);
            glViewport(0, 0, g_width, g_height);
            return 0;
            
        case WM_MOUSEWHEEL:
            g_mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 1 : -1;
            return 0;
            
        case WM_LBUTTONDOWN:
            g_mousePressed = true;
            return 0;
            
        case WM_LBUTTONUP:
            g_mousePressed = false;
            return 0;
            
        case WM_MOUSEMOVE:
            g_mouseX = LOWORD(lParam);
            g_mouseY = HIWORD(lParam);
            return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
#endif 