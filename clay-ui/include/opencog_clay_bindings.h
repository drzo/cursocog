#ifndef OPENCOG_CLAY_BINDINGS_H
#define OPENCOG_CLAY_BINDINGS_H

#include "clay.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the OpenCog Clay UI bindings
 * @return true on success, false on failure
 */
bool OpenCogClayBindings_Initialize();

/**
 * @brief Shutdown the OpenCog Clay UI bindings
 */
void OpenCogClayBindings_Shutdown();

/**
 * @brief Render an AtomSpace as a graph
 * @param boundingBox The box in which to render the graph
 * @param atomSpacePtr Pointer to the AtomSpace (cast to void*)
 */
void OpenCogClayBindings_RenderAtomSpaceGraph(Clay_BoundingBox boundingBox, void* atomSpacePtr);

/**
 * @brief Render details for a selected atom
 * @param boundingBox The box in which to render the details
 * @param atomPtr Pointer to the Atom (cast to void*)
 */
void OpenCogClayBindings_RenderAtomDetails(Clay_BoundingBox boundingBox, void* atomPtr);

/**
 * @brief Callback function type for atom selection
 * @param atomPtr Pointer to the selected Atom
 * @param userData User data passed to the callback
 */
typedef void (*OpenCogClayBindings_AtomSelectedCallback)(void* atomPtr, void* userData);

/**
 * @brief Set callback for atom selection
 * @param callback The callback function
 * @param userData User data to pass to the callback
 */
void OpenCogClayBindings_SetAtomSelectedCallback(OpenCogClayBindings_AtomSelectedCallback callback, void* userData);

#ifdef __cplusplus
}
#endif

#endif // OPENCOG_CLAY_BINDINGS_H 