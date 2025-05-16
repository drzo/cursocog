/*
 * OptimizedGraphRenderer.cpp
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
#include <algorithm>
#include <cmath>
#include <opencog/OptimizedGraphRenderer.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/truthvalue/TruthValue.h>

namespace opencog {

// Quadtree implementation
Quadtree::Quadtree(const AABB& boundary, int depth)
    : _boundary(boundary),
      _depth(depth),
      _count(0),
      _isDivided(false)
{
    // Initialize children as null
    for (auto& child : _children) {
        child = nullptr;
    }
}

Quadtree::~Quadtree()
{
    clear();
}

bool Quadtree::insert(const Node& node)
{
    // Atom doesn't fit in this quad
    if (!_boundary.containsPoint(node.position)) {
        return false;
    }
    
    // If there's space in this quad and we haven't subdivided, add here
    if (_atoms.size() < MAX_ATOMS_PER_NODE && !_isDivided && _depth < MAX_DEPTH) {
        _atoms.push_back(node);
        _count++;
        return true;
    }
    
    // Otherwise, subdivide if needed and insert into children
    if (!_isDivided) {
        subdivide();
    }
    
    // Try to insert into children
    for (auto& child : _children) {
        if (child->insert(node)) {
            _count++;
            return true;
        }
    }
    
    // If we're at max depth, store it here anyway
    if (_depth == MAX_DEPTH) {
        _atoms.push_back(node);
        _count++;
        return true;
    }
    
    return false;
}

void Quadtree::clear()
{
    _atoms.clear();
    for (auto& child : _children) {
        child = nullptr;
    }
    _isDivided = false;
    _count = 0;
}

std::vector<Quadtree::Node> Quadtree::queryRange(const AABB& range) const
{
    std::vector<Node> result;
    
    // Abort if this quad doesn't intersect the range
    if (!_boundary.intersects(range)) {
        return result;
    }
    
    // Check all atoms in this quad
    for (const auto& atom : _atoms) {
        if (range.containsPoint(atom.position)) {
            result.push_back(atom);
        }
    }
    
    // If this quad isn't subdivided, return
    if (!_isDivided) {
        return result;
    }
    
    // Otherwise, collect from children
    for (const auto& child : _children) {
        if (child) {
            auto childResult = child->queryRange(range);
            result.insert(result.end(), childResult.begin(), childResult.end());
        }
    }
    
    return result;
}

void Quadtree::subdivide()
{
    // Already subdivided
    if (_isDivided) return;
    
    Vector2 center = _boundary.center;
    Vector2 halfDim = _boundary.halfDimension * 0.5f;
    
    // Create 4 children (NW, NE, SW, SE)
    _children[0] = std::make_unique<Quadtree>(
        AABB(Vector2(center.x - halfDim.x, center.y + halfDim.y), halfDim),
        _depth + 1
    );
    
    _children[1] = std::make_unique<Quadtree>(
        AABB(Vector2(center.x + halfDim.x, center.y + halfDim.y), halfDim),
        _depth + 1
    );
    
    _children[2] = std::make_unique<Quadtree>(
        AABB(Vector2(center.x - halfDim.x, center.y - halfDim.y), halfDim),
        _depth + 1
    );
    
    _children[3] = std::make_unique<Quadtree>(
        AABB(Vector2(center.x + halfDim.x, center.y - halfDim.y), halfDim),
        _depth + 1
    );
    
    _isDivided = true;
    
    // Move existing atoms to children
    auto atoms = _atoms;
    _atoms.clear();
    
    for (const auto& atom : atoms) {
        // Try to insert in children
        bool inserted = false;
        for (auto& child : _children) {
            if (child->insert(atom)) {
                inserted = true;
                break;
            }
        }
        
        // If couldn't insert in any child, keep it here
        if (!inserted) {
            _atoms.push_back(atom);
        }
    }
}

// OptimizedGraphRenderer implementation
OptimizedGraphRenderer::RenderConfig::RenderConfig()
    : showLabels(true),
      showTruthValues(false),
      showTypes(true),
      edgeThickness(1.0f),
      nodeSize(10.0f),
      labelSize(10.0f),
      backgroundColor(0.1f, 0.1f, 0.1f),
      defaultNodeColor(0.7f, 0.7f, 0.7f),
      defaultEdgeColor(0.5f, 0.5f, 0.5f),
      highlightColor(1.0f, 0.5f, 0.0f),
      selectedColor(1.0f, 1.0f, 0.0f)
{
}

OptimizedGraphRenderer::ViewState::ViewState()
    : position(0.0f, 0.0f),
      scale(1.0f)
{
}

void OptimizedGraphRenderer::ViewState::updateVisibleArea(float viewportWidth, float viewportHeight)
{
    float halfWidth = (viewportWidth / 2.0f) / scale;
    float halfHeight = (viewportHeight / 2.0f) / scale;
    visibleArea = Quadtree::AABB(position, Vector2(halfWidth, halfHeight));
}

OptimizedGraphRenderer::AtomRenderInfo::AtomRenderInfo()
    : size(1.0f),
      isVisible(true),
      isHighlighted(false),
      isSelected(false),
      detailLevel(DetailLevel::FULL)
{
}

OptimizedGraphRenderer::AtomRenderInfo::AtomRenderInfo(const Handle& h)
    : atom(h),
      size(1.0f),
      isVisible(true),
      isHighlighted(false),
      isSelected(false),
      detailLevel(DetailLevel::FULL)
{
    if (h->is_node()) {
        // For nodes, use the name as label
        NodePtr node = NodeCast(h);
        label = node->get_name();
    } else {
        // For links, use the type name
        LinkPtr link = LinkCast(h);
        label = nameserver().getTypeName(link->get_type());
    }
}

OptimizedGraphRenderer::EdgeRenderInfo::EdgeRenderInfo()
    : thickness(1.0f),
      isVisible(true)
{
}

OptimizedGraphRenderer::EdgeRenderInfo::EdgeRenderInfo(const Handle& source, const Handle& target)
    : sourceAtom(source),
      targetAtom(target),
      thickness(1.0f),
      isVisible(true)
{
}

OptimizedGraphRenderer::OptimizedGraphRenderer()
    : _quadtree(Quadtree::AABB(Vector2(0, 0), Vector2(1000, 1000))),
      _visibleAtomCount(0),
      _visibleEdgeCount(0),
      _drawCallCount(0),
      _lastFrameTime(0.0f),
      _detailLevel(DetailLevel::FULL),
      _autoDynamicDetail(true)
{
    // Default node color provider based on atom type
    _nodeColorProvider = [this](const Handle& h) -> Color {
        if (h->is_node()) {
            Type type = h->get_type();
            // Hue based on type (cycle through colors)
            float hue = static_cast<float>(type % 10) / 10.0f;
            
            // Convert HSV to RGB (simplified)
            float r, g, b;
            float h = hue * 6.0f;
            int i = static_cast<int>(h);
            float f = h - i;
            float p = 0.6f; // Value * (1 - Saturation)
            float q = 0.6f * (1.0f - (0.8f * f));
            float t = 0.6f * (1.0f - (0.8f * (1.0f - f)));
            
            switch (i % 6) {
                case 0: r = 0.9f; g = t; b = p; break;
                case 1: r = q; g = 0.9f; b = p; break;
                case 2: r = p; g = 0.9f; b = t; break;
                case 3: r = p; g = q; b = 0.9f; break;
                case 4: r = t; g = p; b = 0.9f; break;
                default: r = 0.9f; g = p; b = q; break;
            }
            
            return Color(r, g, b);
        } else {
            // Links are gray by default
            return _renderConfig.defaultEdgeColor;
        }
    };
    
    // Default edge color provider
    _edgeColorProvider = [this](const Handle& h) -> Color {
        return _renderConfig.defaultEdgeColor;
    };
}

OptimizedGraphRenderer::~OptimizedGraphRenderer()
{
}

void OptimizedGraphRenderer::initialize(int viewportWidth, int viewportHeight)
{
    _viewState.updateVisibleArea(viewportWidth, viewportHeight);
}

void OptimizedGraphRenderer::setAtoms(const HandleSeq& atoms)
{
    // Clear existing data
    _atomRenderInfo.clear();
    _edgeRenderInfo.clear();
    _quadtree.clear();
    
    // Add nodes first
    for (const Handle& h : atoms) {
        if (h->is_node()) {
            addAtom(h);
        }
    }
    
    // Then add links
    for (const Handle& h : atoms) {
        if (h->is_link()) {
            addAtom(h);
        }
    }
    
    // Update the quadtree
    updateQuadtree();
}

void OptimizedGraphRenderer::addAtom(const Handle& h)
{
    // Check if atom already exists
    if (_atomRenderInfo.find(h) != _atomRenderInfo.end()) {
        return;
    }
    
    // Create atom render info
    AtomRenderInfo info(h);
    
    // Calculate position (simplified for now)
    info.position = calculateAtomPosition(h);
    
    // Calculate color
    info.color = calculateAtomColor(h);
    
    // Calculate size based on atom type
    if (h->is_node()) {
        info.size = _renderConfig.nodeSize;
    } else {
        // Links are slightly smaller
        info.size = _renderConfig.nodeSize * 0.8f;
    }
    
    // Add to render info map
    _atomRenderInfo[h] = info;
    
    // If it's a link, create edges
    if (h->is_link()) {
        LinkPtr link = LinkCast(h);
        const HandleSeq& outgoing = link->getOutgoingSet();
        
        // Create edges from this link to each atom in its outgoing set
        for (const Handle& target : outgoing) {
            EdgeRenderInfo edgeInfo(h, target);
            edgeInfo.color = _edgeColorProvider(h);
            edgeInfo.thickness = _renderConfig.edgeThickness;
            
            _edgeRenderInfo.push_back(edgeInfo);
        }
    }
    
    // Update the quadtree
    updateQuadtree();
}

void OptimizedGraphRenderer::removeAtom(const Handle& h)
{
    // Remove atom render info
    _atomRenderInfo.erase(h);
    
    // Remove edges involving this atom
    _edgeRenderInfo.erase(
        std::remove_if(_edgeRenderInfo.begin(), _edgeRenderInfo.end(),
            [&](const EdgeRenderInfo& edge) {
                return edge.sourceAtom == h || edge.targetAtom == h;
            }),
        _edgeRenderInfo.end()
    );
    
    // Rebuild quadtree
    updateQuadtree();
}

void OptimizedGraphRenderer::updateAtom(const Handle& h)
{
    // If atom doesn't exist, add it
    if (_atomRenderInfo.find(h) == _atomRenderInfo.end()) {
        addAtom(h);
        return;
    }
    
    // Update atom information
    _atomRenderInfo[h].color = calculateAtomColor(h);
    
    // If it's a link, update its edges
    if (h->is_link()) {
        // Remove existing edges
        _edgeRenderInfo.erase(
            std::remove_if(_edgeRenderInfo.begin(), _edgeRenderInfo.end(),
                [&](const EdgeRenderInfo& edge) {
                    return edge.sourceAtom == h;
                }),
            _edgeRenderInfo.end()
        );
        
        // Re-add edges
        LinkPtr link = LinkCast(h);
        const HandleSeq& outgoing = link->getOutgoingSet();
        
        for (const Handle& target : outgoing) {
            EdgeRenderInfo edgeInfo(h, target);
            edgeInfo.color = _edgeColorProvider(h);
            edgeInfo.thickness = _renderConfig.edgeThickness;
            
            _edgeRenderInfo.push_back(edgeInfo);
        }
    }
}

void OptimizedGraphRenderer::render(int viewportWidth, int viewportHeight)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Update visible area
    _viewState.updateVisibleArea(viewportWidth, viewportHeight);
    
    // Update detail level if auto-detail is enabled
    if (_autoDynamicDetail) {
        updateDynamicDetailLevel();
    }
    
    // Calculate visibility for atoms and edges
    calculateVisibility();
    
    // Simplify edges for dense regions
    if (_detailLevel != DetailLevel::FULL) {
        float threshold = (_detailLevel == DetailLevel::MEDIUM) ? 0.9f :
                         (_detailLevel == DetailLevel::LOW) ? 0.7f : 0.5f;
        simplifyEdges(threshold);
    }
    
    // Reset draw call counter
    _drawCallCount = 0;
    
    // Render edges
    renderEdges(viewportWidth, viewportHeight);
    
    // Render nodes
    renderNodes(viewportWidth, viewportHeight);
    
    // Render labels if enabled
    if (_renderConfig.showLabels) {
        renderLabels(viewportWidth, viewportHeight);
    }
    
    // Calculate frame time
    auto endTime = std::chrono::high_resolution_clock::now();
    _lastFrameTime = std::chrono::duration<float, std::milli>(endTime - startTime).count();
}

// Helper function to update the quadtree with current atom positions
void OptimizedGraphRenderer::updateQuadtree()
{
    // Clear the quadtree
    _quadtree.clear();
    
    // Calculate bounds
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    
    for (const auto& pair : _atomRenderInfo) {
        const Vector2& pos = pair.second.position;
        minX = std::min(minX, pos.x);
        minY = std::min(minY, pos.y);
        maxX = std::max(maxX, pos.x);
        maxY = std::max(maxY, pos.y);
    }
    
    // Add padding
    float padding = 100.0f;
    minX -= padding;
    minY -= padding;
    maxX += padding;
    maxY += padding;
    
    // Create new quadtree with appropriate bounds
    Vector2 center((minX + maxX) / 2.0f, (minY + maxY) / 2.0f);
    Vector2 halfDim((maxX - minX) / 2.0f, (maxY - minY) / 2.0f);
    _quadtree = Quadtree(Quadtree::AABB(center, halfDim));
    
    // Insert atoms into quadtree
    for (const auto& pair : _atomRenderInfo) {
        Quadtree::Node node(pair.first, pair.second.position, pair.second.size);
        node.color = pair.second.color;
        _quadtree.insert(node);
    }
}

// Calculate which atoms and edges are visible
void OptimizedGraphRenderer::calculateVisibility()
{
    // Reset counters
    _visibleAtomCount = 0;
    _visibleEdgeCount = 0;
    
    // Query quadtree for visible atoms
    std::vector<Quadtree::Node> visibleNodes = _quadtree.queryRange(_viewState.visibleArea);
    
    // Mark all atoms as not visible first
    for (auto& pair : _atomRenderInfo) {
        pair.second.isVisible = false;
    }
    
    // Mark queried atoms as visible
    for (const auto& node : visibleNodes) {
        auto it = _atomRenderInfo.find(node.atom);
        if (it != _atomRenderInfo.end()) {
            it->second.isVisible = true;
            _visibleAtomCount++;
        }
    }
    
    // Check edge visibility - both ends must be visible
    for (auto& edge : _edgeRenderInfo) {
        auto srcIt = _atomRenderInfo.find(edge.sourceAtom);
        auto dstIt = _atomRenderInfo.find(edge.targetAtom);
        
        edge.isVisible = false;
        
        if (srcIt != _atomRenderInfo.end() && dstIt != _atomRenderInfo.end()) {
            if (srcIt->second.isVisible && dstIt->second.isVisible) {
                edge.isVisible = true;
                _visibleEdgeCount++;
            }
        }
    }
}

// Update detail level based on number of visible atoms
void OptimizedGraphRenderer::updateDynamicDetailLevel()
{
    const int HIGH_THRESHOLD = 500;
    const int MEDIUM_THRESHOLD = 1000;
    const int LOW_THRESHOLD = 5000;
    
    int totalVisibleCount = _visibleAtomCount;
    
    if (totalVisibleCount < HIGH_THRESHOLD) {
        _detailLevel = DetailLevel::FULL;
    } else if (totalVisibleCount < MEDIUM_THRESHOLD) {
        _detailLevel = DetailLevel::MEDIUM;
    } else if (totalVisibleCount < LOW_THRESHOLD) {
        _detailLevel = DetailLevel::LOW;
    } else {
        _detailLevel = DetailLevel::MINIMAL;
    }
}

// Simplify edges for better performance
void OptimizedGraphRenderer::simplifyEdges(float threshold)
{
    // Skip if there are few edges
    if (_edgeRenderInfo.size() < 100) return;
    
    // Count how many edges are connected to each atom
    std::unordered_map<Handle, int> edgeCount;
    
    for (const auto& edge : _edgeRenderInfo) {
        if (!edge.isVisible) continue;
        
        edgeCount[edge.sourceAtom]++;
        edgeCount[edge.targetAtom]++;
    }
    
    // Find the maximum edge count to normalize
    int maxCount = 0;
    for (const auto& pair : edgeCount) {
        maxCount = std::max(maxCount, pair.second);
    }
    
    if (maxCount == 0) return;
    
    // Hide edges for atoms with too many connections
    for (auto& edge : _edgeRenderInfo) {
        if (!edge.isVisible) continue;
        
        // Get normalized edge counts for source and target
        float sourceNormCount = static_cast<float>(edgeCount[edge.sourceAtom]) / maxCount;
        float targetNormCount = static_cast<float>(edgeCount[edge.targetAtom]) / maxCount;
        
        // Hide edges that exceed the threshold
        if (sourceNormCount > threshold || targetNormCount > threshold) {
            edge.isVisible = false;
            _visibleEdgeCount--;
        }
    }
}

// Convert world position to screen position
Vector2 OptimizedGraphRenderer::worldToScreen(const Vector2& worldPos, int viewportWidth, int viewportHeight) const
{
    Vector2 centered = worldPos - _viewState.position;
    centered = centered * _viewState.scale;
    
    return Vector2(
        centered.x + viewportWidth / 2.0f,
        centered.y + viewportHeight / 2.0f
    );
}

// Convert screen position to world position
Vector2 OptimizedGraphRenderer::screenToWorld(int screenX, int screenY, int viewportWidth, int viewportHeight) const
{
    Vector2 centered(
        screenX - viewportWidth / 2.0f,
        screenY - viewportHeight / 2.0f
    );
    
    centered = centered / _viewState.scale;
    
    return centered + _viewState.position;
}

// Calculate atom color based on type, truth value, etc.
Color OptimizedGraphRenderer::calculateAtomColor(const Handle& h) const
{
    // Use custom color provider if available
    if (_nodeColorProvider && h->is_node()) {
        return _nodeColorProvider(h);
    }
    
    if (_edgeColorProvider && h->is_link()) {
        return _edgeColorProvider(h);
    }
    
    return h->is_node() ? _renderConfig.defaultNodeColor : _renderConfig.defaultEdgeColor;
}

// Calculate atom position (simplified - in a full implementation, this would use layout algorithms)
Vector2 OptimizedGraphRenderer::calculateAtomPosition(const Handle& h)
{
    // For nodes, use a simple random position if not already placed
    if (h->is_node()) {
        // Use the atom's handle to generate a pseudo-random but consistent position
        std::hash<Handle> hasher;
        size_t hashVal = hasher(h);
        
        float x = static_cast<float>(hashVal % 1000) - 500.0f;
        float y = static_cast<float>((hashVal / 1000) % 1000) - 500.0f;
        
        return Vector2(x, y);
    }
    
    // For links, calculate position based on connected nodes
    if (h->is_link()) {
        LinkPtr link = LinkCast(h);
        const HandleSeq& outgoing = link->getOutgoingSet();
        
        if (outgoing.empty()) {
            // Fallback to random position
            return calculateAtomPosition(Handle::UNDEFINED);
        }
        
        // Calculate average position of connected nodes
        Vector2 avgPos(0.0f, 0.0f);
        int validCount = 0;
        
        for (const Handle& target : outgoing) {
            auto it = _atomRenderInfo.find(target);
            if (it != _atomRenderInfo.end()) {
                avgPos = avgPos + it->second.position;
                validCount++;
            }
        }
        
        if (validCount > 0) {
            avgPos = avgPos * (1.0f / validCount);
        }
        
        return avgPos;
    }
    
    // Default fallback
    return Vector2(0.0f, 0.0f);
}

// Render nodes using batched rendering for performance
void OptimizedGraphRenderer::renderNodes(int viewportWidth, int viewportHeight)
{
    // Get visible atoms from quadtree
    std::vector<Quadtree::Node> visibleNodes = _quadtree.queryRange(_viewState.visibleArea);
    
    // Batch render the nodes
    batchRender(visibleNodes, viewportWidth, viewportHeight);
}

// Optimized batch rendering of nodes
void OptimizedGraphRenderer::batchRender(const std::vector<Quadtree::Node>& nodes, int viewportWidth, int viewportHeight)
{
    // In a real implementation, this would use hardware-accelerated rendering APIs
    // Here, we just track how many draw calls we would make
    if (nodes.empty()) return;
    
    // Sort by node type to minimize state changes
    std::unordered_map<Type, std::vector<Quadtree::Node>> nodesByType;
    
    for (const auto& node : nodes) {
        Handle h = node.atom;
        Type t = h->get_type();
        nodesByType[t].push_back(node);
    }
    
    // Render each batch
    for (const auto& batch : nodesByType) {
        // This would be a single draw call in a real implementation
        _drawCallCount++;
    }
}

// Render edges between nodes
void OptimizedGraphRenderer::renderEdges(int viewportWidth, int viewportHeight)
{
    // In a real implementation, this would use hardware-accelerated rendering APIs
    // Here, we just track statistics
    int visibleEdges = 0;
    
    for (const auto& edge : _edgeRenderInfo) {
        if (!edge.isVisible) continue;
        
        visibleEdges++;
    }
    
    // Count draw calls - in a real implementation, edges would be batched
    _drawCallCount += (visibleEdges + 999) / 1000; // One batch per 1000 edges
}

// Render atom labels
void OptimizedGraphRenderer::renderLabels(int viewportWidth, int viewportHeight)
{
    // Only render labels for certain detail levels
    if (_detailLevel == DetailLevel::MINIMAL) return;
    
    // In a real implementation, this would render text for each visible atom
    // Here, we just track draw calls
    _drawCallCount += 1; // Text batch
}

// Get atom at a specific screen position (for interaction)
Handle OptimizedGraphRenderer::getAtomAt(int screenX, int screenY)
{
    // Convert screen position to world position
    Vector2 worldPos = screenToWorld(screenX, screenY, 1280, 720); // Viewport size hard-coded here for simplicity
    
    // Create small AABB around the point
    float pickRadius = 10.0f / _viewState.scale; // 10 pixel radius
    Quadtree::AABB pickArea(worldPos, Vector2(pickRadius, pickRadius));
    
    // Query the quadtree for atoms in this area
    std::vector<Quadtree::Node> candidateNodes = _quadtree.queryRange(pickArea);
    
    // Find the closest atom
    Handle closestAtom = Handle::UNDEFINED;
    float closestDist = pickRadius;
    
    for (const auto& node : candidateNodes) {
        Vector2 delta = worldPos - node.position;
        float dist = delta.length();
        
        // Check if the point is within the atom's circle
        if (dist <= node.size / 2.0f + pickRadius && dist < closestDist) {
            closestAtom = node.atom;
            closestDist = dist;
        }
    }
    
    return closestAtom;
}

// Camera controls
void OptimizedGraphRenderer::setViewPosition(const Vector2& position)
{
    _viewState.position = position;
}

void OptimizedGraphRenderer::setViewScale(float scale)
{
    _viewState.scale = std::max(0.01f, scale);
}

void OptimizedGraphRenderer::zoomIn(float factor)
{
    _viewState.scale *= factor;
}

void OptimizedGraphRenderer::zoomOut(float factor)
{
    _viewState.scale /= factor;
    _viewState.scale = std::max(0.01f, _viewState.scale);
}

void OptimizedGraphRenderer::resetView()
{
    _viewState.position = Vector2(0.0f, 0.0f);
    _viewState.scale = 1.0f;
}

// Selection management
void OptimizedGraphRenderer::selectAtom(const Handle& h)
{
    // Check if atom exists
    auto it = _atomRenderInfo.find(h);
    if (it == _atomRenderInfo.end()) return;
    
    // Mark as selected
    it->second.isSelected = true;
    
    // Add to selection list if not already there
    if (std::find(_selectedAtoms.begin(), _selectedAtoms.end(), h) == _selectedAtoms.end()) {
        _selectedAtoms.push_back(h);
    }
}

void OptimizedGraphRenderer::deselectAtom(const Handle& h)
{
    // Check if atom exists
    auto it = _atomRenderInfo.find(h);
    if (it == _atomRenderInfo.end()) return;
    
    // Mark as not selected
    it->second.isSelected = false;
    
    // Remove from selection list
    _selectedAtoms.erase(
        std::remove(_selectedAtoms.begin(), _selectedAtoms.end(), h),
        _selectedAtoms.end()
    );
}

void OptimizedGraphRenderer::clearSelection()
{
    // Clear selection flags
    for (auto& pair : _atomRenderInfo) {
        pair.second.isSelected = false;
    }
    
    // Clear selection list
    _selectedAtoms.clear();
}

bool OptimizedGraphRenderer::isAtomSelected(const Handle& h) const
{
    auto it = _atomRenderInfo.find(h);
    return (it != _atomRenderInfo.end() && it->second.isSelected);
}

const HandleSeq& OptimizedGraphRenderer::getSelectedAtoms() const
{
    return _selectedAtoms;
}

// Highlighting
void OptimizedGraphRenderer::highlightAtom(const Handle& h)
{
    // Check if atom exists
    auto it = _atomRenderInfo.find(h);
    if (it == _atomRenderInfo.end()) return;
    
    // Mark as highlighted
    it->second.isHighlighted = true;
    
    // Add to highlight list if not already there
    if (std::find(_highlightedAtoms.begin(), _highlightedAtoms.end(), h) == _highlightedAtoms.end()) {
        _highlightedAtoms.push_back(h);
    }
}

void OptimizedGraphRenderer::unhighlightAtom(const Handle& h)
{
    // Check if atom exists
    auto it = _atomRenderInfo.find(h);
    if (it == _atomRenderInfo.end()) return;
    
    // Mark as not highlighted
    it->second.isHighlighted = false;
    
    // Remove from highlight list
    _highlightedAtoms.erase(
        std::remove(_highlightedAtoms.begin(), _highlightedAtoms.end(), h),
        _highlightedAtoms.end()
    );
}

void OptimizedGraphRenderer::clearHighlights()
{
    // Clear highlight flags
    for (auto& pair : _atomRenderInfo) {
        pair.second.isHighlighted = false;
    }
    
    // Clear highlight list
    _highlightedAtoms.clear();
}

// Configuration
void OptimizedGraphRenderer::setRenderConfig(const RenderConfig& config)
{
    _renderConfig = config;
}

const OptimizedGraphRenderer::RenderConfig& OptimizedGraphRenderer::getRenderConfig() const
{
    return _renderConfig;
}

// Statistics
int OptimizedGraphRenderer::getVisibleAtomCount() const
{
    return _visibleAtomCount;
}

int OptimizedGraphRenderer::getTotalAtomCount() const
{
    return _atomRenderInfo.size();
}

int OptimizedGraphRenderer::getVisibleEdgeCount() const
{
    return _visibleEdgeCount;
}

int OptimizedGraphRenderer::getDrawCallCount() const
{
    return _drawCallCount;
}

float OptimizedGraphRenderer::getLastFrameTime() const
{
    return _lastFrameTime;
}

// Custom coloring
void OptimizedGraphRenderer::setNodeColorProvider(std::function<Color(const Handle&)> provider)
{
    _nodeColorProvider = provider;
}

void OptimizedGraphRenderer::setEdgeColorProvider(std::function<Color(const Handle&)> provider)
{
    _edgeColorProvider = provider;
}

// Dynamic level-of-detail control
void OptimizedGraphRenderer::setDetailLevel(DetailLevel level)
{
    _detailLevel = level;
}

OptimizedGraphRenderer::DetailLevel OptimizedGraphRenderer::getDetailLevel() const
{
    return _detailLevel;
}

void OptimizedGraphRenderer::setAutoDynamicDetail(bool enable)
{
    _autoDynamicDetail = enable;
}

bool OptimizedGraphRenderer::isAutoDynamicDetail() const
{
    return _autoDynamicDetail;
}

} // namespace opencog
 