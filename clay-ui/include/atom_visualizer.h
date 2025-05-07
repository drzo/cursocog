#ifndef ATOM_VISUALIZER_H
#define ATOM_VISUALIZER_H

#include "opencog_clay_bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Renders an AtomSpace graph within the specified bounding box
 * 
 * @param boundingBox The box in which to render the graph
 * @param atomSpacePtr Pointer to the AtomSpace to visualize (cast to void*)
 */
void OpenCogClayBindings_RenderAtomSpaceGraph(Clay_BoundingBox boundingBox, void* atomSpacePtr);

/**
 * Selects an atom in the visualization
 * 
 * @param atomId The ID of the atom to select
 */
void OpenCogClayBindings_SelectAtom(const char* atomId);

/**
 * Fixes or unfixes an atom's position in the visualization
 * 
 * @param atomId The ID of the atom to fix/unfix
 * @param fixed True to fix the position, false to allow movement
 */
void OpenCogClayBindings_FixAtomPosition(const char* atomId, bool fixed);

/**
 * Runs the graph physics simulation for a number of steps
 * 
 * @param steps Number of simulation steps to run (default: 100)
 */
void OpenCogClayBindings_RunGraphSimulation(int steps);

/**
 * Creates a demo AtomSpace for visualization testing
 */
void OpenCogClayBindings_CreateDemoAtomSpace();

#ifdef __cplusplus
}
#endif

#endif // ATOM_VISUALIZER_H 