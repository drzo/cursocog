#ifndef OPENCOG_ATOMSPACE_BRIDGE_H
#define OPENCOG_ATOMSPACE_BRIDGE_H

/**
 * @file opencog_atomspace_bridge.h
 * @brief Bridge between Clay UI and OpenCog AtomSpace
 * 
 * This file provides a bridge between the Clay UI visualization system
 * and the OpenCog AtomSpace. It allows visualizing AtomSpace contents
 * in real-time using the Clay UI system.
 */

#include "opencog_clay_bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the AtomSpace bridge
 * @param storageNodeType Type of storage node to connect to ("rocks", "cogserver", "postgres")
 * @param uri URI for the storage connection
 * @return true on success, false on failure
 */
bool OpenCogAtomSpaceBridge_Initialize(const char* storageNodeType, const char* uri);

/**
 * @brief Shutdown the AtomSpace bridge
 */
void OpenCogAtomSpaceBridge_Shutdown();

/**
 * @brief Connect to an AtomSpace via a StorageNode
 * @param storageNodeType Type of storage node to connect to
 * @param uri URI for the storage connection
 * @return true on success, false on failure
 */
bool OpenCogAtomSpaceBridge_Connect(const char* storageNodeType, const char* uri);

/**
 * @brief Disconnect from the AtomSpace
 */
void OpenCogAtomSpaceBridge_Disconnect();

/**
 * @brief Fetch atoms of the specified type
 * @param type Atom type name (e.g., "ConceptNode", "EvaluationLink")
 * @param recursive Whether to fetch recursively
 * @return Number of atoms fetched
 */
int OpenCogAtomSpaceBridge_FetchByType(const char* type, bool recursive);

/**
 * @brief Fetch atoms matching a pattern
 * @param patternString AtomSpace query pattern in Scheme format
 * @return Number of atoms fetched
 */
int OpenCogAtomSpaceBridge_FetchByPattern(const char* patternString);

/**
 * @brief Get the current AtomSpace as a graph for visualization
 * @return Pointer to the graph data structure for Clay UI rendering
 */
void* OpenCogAtomSpaceBridge_GetVisualizationGraph();

/**
 * @brief Update the visualization with live AtomSpace changes
 * @return Number of updates made
 */
int OpenCogAtomSpaceBridge_UpdateVisualization();

/**
 * @brief Execute an AtomSpace query and visualize the results
 * @param queryString AtomSpace query in Scheme format
 * @return Number of results found
 */
int OpenCogAtomSpaceBridge_ExecuteQuery(const char* queryString);

/**
 * @brief Add a monitor for AtomSpace changes
 * @param callback Function to call when AtomSpace changes
 * @param userData User data to pass to the callback
 * @return Monitor ID for removing the monitor later
 */
int OpenCogAtomSpaceBridge_AddChangeMonitor(void (*callback)(void* atomPtr, void* userData), void* userData);

/**
 * @brief Remove a monitor for AtomSpace changes
 * @param monitorId Monitor ID returned from AddChangeMonitor
 * @return true on success, false on failure
 */
bool OpenCogAtomSpaceBridge_RemoveChangeMonitor(int monitorId);

#ifdef __cplusplus
}
#endif

#endif // OPENCOG_ATOMSPACE_BRIDGE_H 