/*
 * RealTimeAtomSpaceVisualizer.h
 *
 * Copyright (C) 2025 OpenCog Foundation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_REAL_TIME_ATOMSPACE_VISUALIZER_H
#define _OPENCOG_REAL_TIME_ATOMSPACE_VISUALIZER_H

#include <mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <functional>

namespace opencog {

class AtomSpace;
class Handle;
class Atom;

/**
 * RealTimeAtomSpaceVisualizer - Provides real-time visualization of changes to the AtomSpace
 *
 * This class implements a subscription model for the AtomSpace, registering for
 * change notifications and rendering updates to the visualization in real-time.
 * It supports multiple rendering modes, filtering, and search functionality.
 */
class RealTimeAtomSpaceVisualizer {
public:
    enum class LayoutMode {
        FORCE_DIRECTED,   // Force-directed graph layout algorithm
        HIERARCHICAL,     // Tree-like hierarchical layout
        RADIAL,           // Radial/circular layout
        GRID              // Simple grid layout
    };

    enum class ColorMode {
        TYPE_BASED,       // Color nodes/edges based on their types
        TRUTH_VALUE,      // Color based on truth value strengths
        ATTENTION_VALUE,  // Color based on attention values
        CUSTOM            // Custom coloring scheme
    };

    enum class ChangeType {
        ADDED,            // New atom added
        REMOVED,          // Atom removed
        MODIFIED,         // Atom modified (e.g., TV change)
        CONNECTED         // New link connected to this atom
    };

    struct ChangeEvent {
        Handle handle;
        ChangeType type;
        unsigned long timestamp;
    };

    // Constructor and destructor
    RealTimeAtomSpaceVisualizer();
    ~RealTimeAtomSpaceVisualizer();

    // AtomSpace connection
    void connectToAtomSpace(AtomSpacePtr atomspace);
    void disconnectFromAtomSpace();
    bool isConnected() const;

    // Visualization settings
    void setLayoutMode(LayoutMode mode);
    LayoutMode getLayoutMode() const;
    
    void setColorMode(ColorMode mode);
    ColorMode getColorMode() const;
    
    void setNodeSizeFunction(std::function<float(const Handle&)> sizeFunc);
    void setEdgeThicknessFunction(std::function<float(const Handle&)> thicknessFunc);
    
    void setMaxVisibleNodes(size_t maxNodes);
    size_t getMaxVisibleNodes() const;
    
    // Filtering
    void setTypeFilter(const std::vector<Type>& types, bool includeSubtypes = true);
    void clearTypeFilter();
    
    void setTruthValueFilter(float minConfidence, float minStrength);
    void clearTruthValueFilter();

    // Search functionality
    void searchByName(const std::string& namePattern);
    void highlightAtom(const Handle& h, bool highlight = true);
    void clearHighlighting();
    
    // Camera controls
    void zoomToFit();
    void zoomIn();
    void zoomOut();
    void panTo(const Handle& h);
    
    // Export visualization
    bool exportToPNG(const std::string& filename);
    bool exportToSVG(const std::string& filename);
    bool exportToJSON(const std::string& filename);
    
    // Event handling
    void processPendingEvents();
    size_t getPendingEventCount() const;
    
    // Statistics
    size_t getVisibleNodeCount() const;
    size_t getVisibleLinkCount() const;
    size_t getTotalNodeCount() const;
    size_t getTotalLinkCount() const;
    
private:
    // AtomSpace reference
    AtomSpacePtr _atomspace;
    
    // Change notification handling
    std::mutex _eventMutex;
    std::queue<ChangeEvent> _pendingEvents;
    std::atomic<bool> _processingEvents;
    std::thread _updateThread;
    
    // Visualization state
    LayoutMode _layoutMode;
    ColorMode _colorMode;
    size_t _maxVisibleNodes;
    
    // View state
    float _scale;
    float _translateX, _translateY;
    
    // Filtering state
    std::vector<Type> _typeFilter;
    bool _includeSubtypes;
    float _minConfidence;
    float _minStrength;
    
    // Highlighting
    std::unordered_map<Handle, bool> _highlightedAtoms;
    
    // Custom sizing functions
    std::function<float(const Handle&)> _nodeSizeFunc;
    std::function<float(const Handle&)> _edgeThicknessFunc;
    
    // Helper functions
    void startUpdateThread();
    void stopUpdateThread();
    void updateVisualization();
    bool isAtomVisible(const Handle& h) const;
    void handleAtomAdded(const Handle& h);
    void handleAtomRemoved(const Handle& h);
    void handleAtomModified(const Handle& h);
    
    // Physics simulation for force-directed layout
    void updateForces();
    void applyForceDirectedLayout();
    void applyHierarchicalLayout();
    void applyRadialLayout();
    void applyGridLayout();
};

} // namespace opencog

#endif // _OPENCOG_REAL_TIME_ATOMSPACE_VISUALIZER_H 