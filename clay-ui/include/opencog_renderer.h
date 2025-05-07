#ifndef OPENCOG_RENDERER_H
#define OPENCOG_RENDERER_H

#include "clay.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the OpenCog renderer for Clay UI
 * @param width Initial window width
 * @param height Initial window height
 * @param title Window title
 * @return true on success, false on failure
 */
bool OpenCogRenderer_Initialize(uint32_t width, uint32_t height, const char* title);

/**
 * Shutdown the OpenCog renderer
 */
void OpenCogRenderer_Shutdown();

/**
 * Render the Clay UI commands
 * @param commands Array of render commands from Clay_EndLayout()
 */
void OpenCogRenderer_Render(Clay_RenderCommandArray commands);

/**
 * Handle events and update the Clay UI
 * @return true to continue running, false to quit
 */
bool OpenCogRenderer_Update();

/**
 * Get the screen width
 * @return Current screen width
 */
uint32_t OpenCogRenderer_GetWidth();

/**
 * Get the screen height
 * @return Current screen height
 */
uint32_t OpenCogRenderer_GetHeight();

/**
 * Measure text for Clay UI
 * @param text Text to measure
 * @param config Text configuration
 * @param userData User data (unused)
 * @return Dimensions of the text
 */
Clay_Dimensions OpenCogRenderer_MeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, uintptr_t userData);

#ifdef __cplusplus
}
#endif

#endif // OPENCOG_RENDERER_H 