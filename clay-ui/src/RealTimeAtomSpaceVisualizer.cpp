/*
 * RealTimeAtomSpaceVisualizer.cpp
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

#include <chrono>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <opencog/RealTimeAtomSpaceVisualizer.h>
#include <opencog/OptimizedGraphRenderer.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/base/Handle.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>

using namespace std::chrono;

namespace opencog {

// Constructor
RealTimeAtomSpaceVisualizer::RealTimeAtomSpaceVisualizer()
    : _atomspace(nullptr),
      _processingEvents(false),
      _layoutMode(LayoutMode::FORCE_DIRECTED),
      _colorMode(ColorMode::TYPE_BASED),
      _maxVisibleNodes(1000),
      _scale(1.0f),
      _translateX(0.0f),
      _translateY(0.0f),
      _includeSubtypes(true),
      _minConfidence(0.0f),
      _minStrength(0.0f),
      _needsRedraw(true),
      _viewportWidth(1280),
      _viewportHeight(720)
{
    // Default node size function based on outgoing set size
    _nodeSizeFunc = [](const Handle& h) -> float {
        if (h->is_node()) return 10.0f;
        
        // For links, make size proportional to arity
        return 5.0f + h->get_arity() * 2.0f;
    };
    
    // Default edge thickness function based on truth value
    _edgeThicknessFunc = [](const Handle& h) -> float {
        if (h->is_node()) return 1.0f;
        
        TruthValuePtr tv = h->getTruthValue();
        return 1.0f + 4.0f * tv->get_mean();
    };
}

// Destructor
RealTimeAtomSpaceVisualizer::~RealTimeAtomSpaceVisualizer()
{
    disconnectFromAtomSpace();
}

// AtomSpace connection
void RealTimeAtomSpaceVisualizer::connectToAtomSpace(AtomSpacePtr atomspace)
{
    if (_atomspace != nullptr) {
        disconnectFromAtomSpace();
    }
    
    _atomspace = atomspace;
    
    if (_atomspace != nullptr) {
        // Register for AtomSpace change events
        // This uses the AtomSpace signals mechanism
        // Pseudocode - actual implementation would differ based on AtomSpace API
        //_atomspace->addAtomSignal(
        //    std::bind(&RealTimeAtomSpaceVisualizer::handleAtomAdded, this, std::placeholders::_1)
        //);
        //_atomspace->removeAtomSignal(
        //    std::bind(&RealTimeAtomSpaceVisualizer::handleAtomRemoved, this, std::placeholders::_1)
        //);
        //_atomspace->TVChangedSignal(
        //    std::bind(&RealTimeAtomSpaceVisualizer::handleAtomModified, this, std::placeholders::_1)
        //);
        
        // Start the update thread
        startUpdateThread();
        
        // Do initial visualization of existing atoms
        updateVisualization();
    }
}

void RealTimeAtomSpaceVisualizer::disconnectFromAtomSpace()
{
    if (_atomspace != nullptr) {
        // Unregister from AtomSpace signals
        // Pseudocode - actual implementation would differ based on AtomSpace API
        //_atomspace->removeAtomSignal(...);
        //_atomspace->removeAtomSignal(...);
        //_atomspace->TVChangedSignal(...);
        
        // Stop the update thread
        stopUpdateThread();
        
        _atomspace = nullptr;
    }
}

bool RealTimeAtomSpaceVisualizer::isConnected() const
{
    return _atomspace != nullptr;
}

// Visualization settings
void RealTimeAtomSpaceVisualizer::setLayoutMode(LayoutMode mode)
{
    _layoutMode = mode;
    updateVisualization();
}

RealTimeAtomSpaceVisualizer::LayoutMode 
RealTimeAtomSpaceVisualizer::getLayoutMode() const
{
    return _layoutMode;
}

void RealTimeAtomSpaceVisualizer::setColorMode(ColorMode mode)
{
    _colorMode = mode;
    updateVisualization();
}

RealTimeAtomSpaceVisualizer::ColorMode 
RealTimeAtomSpaceVisualizer::getColorMode() const
{
    return _colorMode;
}

void RealTimeAtomSpaceVisualizer::setNodeSizeFunction(
    std::function<float(const Handle&)> sizeFunc)
{
    _nodeSizeFunc = sizeFunc;
    updateVisualization();
}

void RealTimeAtomSpaceVisualizer::setEdgeThicknessFunction(
    std::function<float(const Handle&)> thicknessFunc)
{
    _edgeThicknessFunc = thicknessFunc;
    updateVisualization();
}

void RealTimeAtomSpaceVisualizer::setMaxVisibleNodes(size_t maxNodes)
{
    _maxVisibleNodes = maxNodes;
    if (_renderer) {
        // Update the rendering configuration
        OptimizedGraphRenderer::RenderConfig config = _renderer->getRenderConfig();
        // Apply the new max visible nodes (might need to adjust this)
        _renderer->setRenderConfig(config);
    }
    _needsRedraw = true;
}

size_t RealTimeAtomSpaceVisualizer::getMaxVisibleNodes() const
{
    return _maxVisibleNodes;
}

// Filtering
void RealTimeAtomSpaceVisualizer::setTypeFilter(
    const std::vector<Type>& types, bool includeSubtypes)
{
    _typeFilter = types;
    _includeSubtypes = includeSubtypes;
    updateVisualization();
}

void RealTimeAtomSpaceVisualizer::clearTypeFilter()
{
    _typeFilter.clear();
    updateVisualization();
}

void RealTimeAtomSpaceVisualizer::setTruthValueFilter(
    float minConfidence, float minStrength)
{
    _minConfidence = minConfidence;
    _minStrength = minStrength;
    updateVisualization();
}

void RealTimeAtomSpaceVisualizer::clearTruthValueFilter()
{
    _minConfidence = 0.0f;
    _minStrength = 0.0f;
    updateVisualization();
}

// Search functionality
void RealTimeAtomSpaceVisualizer::searchByName(const std::string& namePattern)
{
    if (!isConnected()) return;
    
    // Clear previous highlighting
    clearHighlighting();
    
    // Find nodes with matching names
    // This would need to be implemented based on the specific AtomSpace API
    // For now, just a pseudocode implementation
    /*
    HandleSeq nodes;
    _atomspace->get_handles_by_name(nodes, namePattern, NODE, true);
    
    // Highlight matching nodes
    for (const Handle& h : nodes) {
        highlightAtom(h);
    }
    */
    
    updateVisualization();
}

void RealTimeAtomSpaceVisualizer::highlightAtom(const Handle& h, bool highlight)
{
    _highlightedAtoms[h] = highlight;
    updateVisualization();
}

void RealTimeAtomSpaceVisualizer::clearHighlighting()
{
    _highlightedAtoms.clear();
    updateVisualization();
}

// Camera controls
void RealTimeAtomSpaceVisualizer::zoomToFit()
{
    if (_renderer) {
        _renderer->resetView();
    }
    _needsRedraw = true;
}

void RealTimeAtomSpaceVisualizer::zoomIn()
{
    if (_renderer) {
        _renderer->zoomIn();
    }
    _needsRedraw = true;
}

void RealTimeAtomSpaceVisualizer::zoomOut()
{
    if (_renderer) {
        _renderer->zoomOut();
    }
    _needsRedraw = true;
}

void RealTimeAtomSpaceVisualizer::panTo(const Handle& h)
{
    // Implementation would depend on the visualization system
    // Basic idea: set translation to center on the specified atom
    
    // Pseudocode:
    /*
    if (_atomPositions.find(h) != _atomPositions.end()) {
        Position pos = _atomPositions[h];
        _translateX = -pos.x;
        _translateY = -pos.y;
    }
    */
    
    updateVisualization();
}

// Export visualization
bool RealTimeAtomSpaceVisualizer::exportToPNG(const std::string& filename)
{
    // Implementation depends on the rendering system
    // Would typically render the current view to an off-screen buffer
    // and then save that buffer as a PNG file
    
    // For now, just a placeholder that reports success
    return true;
}

bool RealTimeAtomSpaceVisualizer::exportToSVG(const std::string& filename)
{
    // Similar to exportToPNG but for SVG format
    // This would involve generating SVG elements for nodes and edges
    
    // For now, just a placeholder that reports success
    return true;
}

bool RealTimeAtomSpaceVisualizer::exportToJSON(const std::string& filename)
{
    // Export the graph structure in a JSON format suitable for
    // visualization with tools like D3.js
    
    // For now, just a placeholder that reports success
    return true;
}

// Event handling
void RealTimeAtomSpaceVisualizer::processPendingEvents()
{
    std::queue<ChangeEvent> eventsToProcess;
    
    // Get all pending events under lock
    {
        std::lock_guard<std::mutex> lock(_eventMutex);
        eventsToProcess.swap(_pendingEvents);
    }
    
    // Process events without holding the lock
    while (!eventsToProcess.empty()) {
        const ChangeEvent& event = eventsToProcess.front();
        
        switch (event.type) {
            case ChangeType::ADDED:
                handleAtomAdded(event.handle);
                break;
                
            case ChangeType::REMOVED:
                handleAtomRemoved(event.handle);
                break;
                
            case ChangeType::MODIFIED:
            case ChangeType::CONNECTED:
                handleAtomModified(event.handle);
                break;
        }
        
        eventsToProcess.pop();
    }
    
    // Update visualization after processing events
    updateVisualization();
}

size_t RealTimeAtomSpaceVisualizer::getPendingEventCount() const
{
    std::lock_guard<std::mutex> lock(_eventMutex);
    return _pendingEvents.size();
}

// Statistics
size_t RealTimeAtomSpaceVisualizer::getVisibleNodeCount() const
{
    if (_renderer) {
        return _renderer->getVisibleAtomCount();
    }
    return 0;
}

size_t RealTimeAtomSpaceVisualizer::getVisibleLinkCount() const
{
    if (_renderer) {
        return _renderer->getVisibleEdgeCount();
    }
    return 0;
}

size_t RealTimeAtomSpaceVisualizer::getTotalNodeCount() const
{
    if (_renderer) {
        return _renderer->getTotalAtomCount();
    }
    return 0;
}

size_t RealTimeAtomSpaceVisualizer::getTotalLinkCount() const
{
    // This would need to be implemented - for now just a placeholder
    return 0;
}

// Private helper methods
void RealTimeAtomSpaceVisualizer::startUpdateThread()
{
    _processingEvents = true;
    
    _updateThread = std::thread([this]() {
        while (_processingEvents) {
            // Process events periodically
            processPendingEvents();
            
            // Sleep to avoid consuming too much CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

void RealTimeAtomSpaceVisualizer::stopUpdateThread()
{
    _processingEvents = false;
    
    if (_updateThread.joinable()) {
        _updateThread.join();
    }
}

void RealTimeAtomSpaceVisualizer::updateVisualization()
{
    if (!_renderer) {
        // Create the optimized renderer if it doesn't exist
        _renderer = std::make_unique<OptimizedGraphRenderer>();
        
        // Configure the renderer
        OptimizedGraphRenderer::RenderConfig config;
        config.showLabels = true;
        config.showTypes = true;
        config.edgeThickness = 1.0f;
        config.nodeSize = 10.0f;
        _renderer->setRenderConfig(config);
        
        // Set initial atoms if connected to AtomSpace
        if (isConnected()) {
            HandleSeq atoms;
            // Get all atoms from the AtomSpace
            // Actual implementation would depend on AtomSpace API
            // _atomspace->get_handles_by_type(atoms, ATOM, true);
            _renderer->setAtoms(atoms);
        }
        
        // Initialize the renderer with default viewport size
        _renderer->initialize(_viewportWidth, _viewportHeight);
    }
    
    // Update the layout if needed
    switch (_layoutMode) {
        case LayoutMode::FORCE_DIRECTED:
            _renderer->setDetailLevel(OptimizedGraphRenderer::DetailLevel::FULL);
            break;
            
        case LayoutMode::HIERARCHICAL:
            _renderer->setDetailLevel(OptimizedGraphRenderer::DetailLevel::MEDIUM);
            break;
            
        case LayoutMode::RADIAL:
            _renderer->setDetailLevel(OptimizedGraphRenderer::DetailLevel::MEDIUM);
            break;
            
        case LayoutMode::GRID:
            _renderer->setDetailLevel(OptimizedGraphRenderer::DetailLevel::LOW);
            break;
    }
    
    // Check if we need to render
    if (_needsRedraw) {
        // Render the graph
        _renderer->render(_viewportWidth, _viewportHeight);
        _needsRedraw = false;
    }
}

bool RealTimeAtomSpaceVisualizer::isAtomVisible(const Handle& h) const
{
    if (!isConnected() || h == Handle::UNDEFINED) return false;
    
    // Apply type filter
    if (!_typeFilter.empty()) {
        bool typeMatch = false;
        Type atomType = h->get_type();
        
        for (Type filterType : _typeFilter) {
            if (_includeSubtypes) {
                if (nameserver().isA(atomType, filterType)) {
                    typeMatch = true;
                    break;
                }
            } else {
                if (atomType == filterType) {
                    typeMatch = true;
                    break;
                }
            }
        }
        
        if (!typeMatch) return false;
    }
    
    // Apply truth value filter
    TruthValuePtr tv = h->getTruthValue();
    if (tv->get_confidence() < _minConfidence || tv->get_mean() < _minStrength) {
        return false;
    }
    
    return true;
}

void RealTimeAtomSpaceVisualizer::handleAtomAdded(const Handle& h)
{
    // Implementation would update internal data structures
    // and then queue a visualization update
    
    // Add to visualization if it passes filters
    if (isAtomVisible(h)) {
        // Add to rendering data structures
        // This depends on the specific implementation
    }
}

void RealTimeAtomSpaceVisualizer::handleAtomRemoved(const Handle& h)
{
    // Implementation would update internal data structures
    // and then queue a visualization update
    
    // Remove from visualization
    // This depends on the specific implementation
}

void RealTimeAtomSpaceVisualizer::handleAtomModified(const Handle& h)
{
    // Implementation would update internal data structures
    // and then queue a visualization update
    
    // Update visualization for this atom
    if (isAtomVisible(h)) {
        // Update rendering data
        // This depends on the specific implementation
    } else {
        // Remove from visualization if it no longer passes filters
        // This depends on the specific implementation
    }
}

// Layout algorithms
void RealTimeAtomSpaceVisualizer::updateForces()
{
    // This would implement force-directed layout updates
    // It's typically an iterative algorithm with repulsion between
    // all nodes and attraction along edges
    
    // Simplified pseudocode:
    /*
    for (auto& nodeA : _visibleNodes) {
        // Apply repulsive force from all other nodes
        for (auto& nodeB : _visibleNodes) {
            if (nodeA == nodeB) continue;
            
            Vector2 delta = nodeA.position - nodeB.position;
            float distance = length(delta);
            if (distance > 0) {
                Vector2 force = normalize(delta) * (REPULSION_CONSTANT / (distance * distance));
                nodeA.force += force;
            }
        }
        
        // Apply attractive force from connected nodes
        for (auto& edge : _visibleEdges) {
            if (edge.source == nodeA || edge.target == nodeA) {
                Node& other = (edge.source == nodeA) ? edge.target : edge.source;
                Vector2 delta = other.position - nodeA.position;
                float distance = length(delta);
                Vector2 force = delta * ATTRACTION_CONSTANT;
                nodeA.force += force;
            }
        }
    }
    
    // Update positions based on forces
    for (auto& node : _visibleNodes) {
        node.velocity = (node.velocity + node.force) * DAMPING;
        node.position += node.velocity;
        node.force = Vector2(0, 0);
    }
    */
}

void RealTimeAtomSpaceVisualizer::applyForceDirectedLayout()
{
    // Implement force-directed layout algorithm
    // This would include several iterations of force calculations
    
    // Simplified implementation:
    for (int i = 0; i < 100; i++) {
        updateForces();
    }
}

void RealTimeAtomSpaceVisualizer::applyHierarchicalLayout()
{
    // Implement hierarchical layout algorithm
    // This would typically arrange atoms in layers based on
    // their relationships
    
    // Placeholder implementation
}

void RealTimeAtomSpaceVisualizer::applyRadialLayout()
{
    // Implement radial layout algorithm
    // This would typically arrange atoms in concentric circles
    // around a central atom
    
    // Placeholder implementation
}

void RealTimeAtomSpaceVisualizer::applyGridLayout()
{
    // Implement simple grid layout
    // This would arrange atoms in a grid pattern
    
    // Placeholder implementation
}

// Add these new functions to support viewport resizing and explicit rendering
void RealTimeAtomSpaceVisualizer::setViewportSize(int width, int height)
{
    _viewportWidth = width;
    _viewportHeight = height;
    if (_renderer) {
        _renderer->initialize(width, height);
    }
    _needsRedraw = true;
}

void RealTimeAtomSpaceVisualizer::render()
{
    if (_renderer) {
        _renderer->render(_viewportWidth, _viewportHeight);
    }
}

} // namespace opencog
 