/*
 * OptimizedGraphRenderer.h
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

#ifndef _OPENCOG_OPTIMIZED_GRAPH_RENDERER_H
#define _OPENCOG_OPTIMIZED_GRAPH_RENDERER_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <array>
#include <functional>

#include <opencog/atoms/base/Handle.h>
#include <opencog/atomspace/AtomSpace.h>

namespace opencog {

/**
 * Represents a 2D point in space
 */
struct Vector2 {
    float x, y;
    
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}
    
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
    
    float length() const {
        return std::sqrt(x * x + y * y);
    }
    
    Vector2 normalized() const {
        float len = length();
        if (len > 0.0001f) {
            return Vector2(x / len, y / len);
        }
        return Vector2(0, 0);
    }
};

/**
 * Represents a color with RGBA components
 */
struct Color {
    float r, g, b, a;
    
    Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color(float r_, float g_, float b_, float a_ = 1.0f) : r(r_), g(g_), b(b_), a(a_) {}
};

/**
 * Quadtree implementation for spatial partitioning
 */
class Quadtree {
public:
    struct AABB {
        Vector2 center;
        Vector2 halfDimension;
        
        AABB() {}
        AABB(const Vector2& center_, const Vector2& halfDim_) 
            : center(center_), halfDimension(halfDim_) {}
        
        bool containsPoint(const Vector2& point) const {
            return (point.x >= center.x - halfDimension.x &&
                    point.x <= center.x + halfDimension.x &&
                    point.y >= center.y - halfDimension.y &&
                    point.y <= center.y + halfDimension.y);
        }
        
        bool intersects(const AABB& other) const {
            return !(center.x + halfDimension.x < other.center.x - other.halfDimension.x ||
                     center.x - halfDimension.x > other.center.x + other.halfDimension.x ||
                     center.y + halfDimension.y < other.center.y - other.halfDimension.y ||
                     center.y - halfDimension.y > other.center.y + other.halfDimension.y);
        }
    };
    
    struct Node {
        Handle atom;
        Vector2 position;
        float size;
        Color color;
        
        Node() : size(1.0f) {}
        Node(const Handle& h, const Vector2& pos, float sz = 1.0f)
            : atom(h), position(pos), size(sz) {}
    };
    
    // Maximum number of atoms per quadtree node
    static const int MAX_ATOMS_PER_NODE = 8;
    // Maximum depth of the quadtree
    static const int MAX_DEPTH = 8;
    
    Quadtree(const AABB& boundary, int depth = 0);
    ~Quadtree();
    
    bool insert(const Node& node);
    void clear();
    std::vector<Node> queryRange(const AABB& range) const;
    int getCount() const { return _count; }
    
private:
    AABB _boundary;
    int _depth;
    int _count;
    std::vector<Node> _atoms;
    std::array<std::unique_ptr<Quadtree>, 4> _children;
    bool _isDivided;
    
    void subdivide();
};

/**
 * OptimizedGraphRenderer - Provides high-performance rendering for large AtomSpace graphs
 *
 * This class implements several optimization techniques:
 * 1. Spatial partitioning with quadtrees for efficient node lookup
 * 2. Level-of-detail rendering based on view distance
 * 3. Frustum culling to only render visible elements
 * 4. Node batching to minimize draw calls
 * 5. Edge simplification for dense graphs
 */
class OptimizedGraphRenderer {
public:
    // Detail level enumeration
    enum class DetailLevel {
        FULL,       // Render everything in full detail
        MEDIUM,     // Render with moderate simplification
        LOW,        // Render with high simplification
        MINIMAL     // Render only the most important elements
    };
    
    // Style configuration
    struct RenderConfig {
        bool showLabels;          // Whether to show atom labels
        bool showTruthValues;     // Whether to show truth values
        bool showTypes;           // Whether to show atom types
        float edgeThickness;      // Thickness of edges
        float nodeSize;           // Base size of nodes
        float labelSize;          // Size of labels
        Color backgroundColor;    // Background color
        Color defaultNodeColor;   // Default node color
        Color defaultEdgeColor;   // Default edge color
        Color highlightColor;     // Color for highlighted atoms
        Color selectedColor;      // Color for selected atoms
        
        RenderConfig();
    };
    
    // Camera/view state
    struct ViewState {
        Vector2 position;     // Camera position
        float scale;          // Zoom level
        AABB visibleArea;     // Visible area in world space
        
        ViewState();
        
        // Update visible area based on camera position and scale
        void updateVisibleArea(float viewportWidth, float viewportHeight);
    };
    
    OptimizedGraphRenderer();
    ~OptimizedGraphRenderer();
    
    // Initialize the renderer
    void initialize(int viewportWidth, int viewportHeight);
    
    // Set atoms to render (rebuild spatial partitioning)
    void setAtoms(const HandleSeq& atoms);
    
    // Add/remove/update atoms
    void addAtom(const Handle& h);
    void removeAtom(const Handle& h);
    void updateAtom(const Handle& h);
    
    // Rendering
    void render(int viewportWidth, int viewportHeight);
    
    // Camera controls
    void setViewPosition(const Vector2& position);
    void setViewScale(float scale);
    void zoomIn(float factor = 1.2f);
    void zoomOut(float factor = 1.2f);
    void resetView();
    
    // Get atom at screen position (for interaction)
    Handle getAtomAt(int screenX, int screenY);
    
    // Selection management
    void selectAtom(const Handle& h);
    void deselectAtom(const Handle& h);
    void clearSelection();
    bool isAtomSelected(const Handle& h) const;
    const HandleSeq& getSelectedAtoms() const;
    
    // Highlighting
    void highlightAtom(const Handle& h);
    void unhighlightAtom(const Handle& h);
    void clearHighlights();
    
    // Configuration
    void setRenderConfig(const RenderConfig& config);
    const RenderConfig& getRenderConfig() const;
    
    // Statistics
    int getVisibleAtomCount() const;
    int getTotalAtomCount() const;
    int getVisibleEdgeCount() const;
    int getDrawCallCount() const;
    float getLastFrameTime() const;
    
    // Custom coloring
    void setNodeColorProvider(std::function<Color(const Handle&)> provider);
    void setEdgeColorProvider(std::function<Color(const Handle&)> provider);
    
    // Dynamic level-of-detail control
    void setDetailLevel(DetailLevel level);
    DetailLevel getDetailLevel() const;
    void setAutoDynamicDetail(bool enable);
    bool isAutoDynamicDetail() const;
    
private:
    // Atom rendering information
    struct AtomRenderInfo {
        Handle atom;
        Vector2 position;
        Vector2 velocity;
        Vector2 force;
        float size;
        Color color;
        bool isVisible;
        bool isHighlighted;
        bool isSelected;
        DetailLevel detailLevel;
        std::string label;
        
        AtomRenderInfo();
        AtomRenderInfo(const Handle& h);
    };
    
    // Edge rendering information
    struct EdgeRenderInfo {
        Handle sourceAtom;
        Handle targetAtom;
        Color color;
        float thickness;
        bool isVisible;
        
        EdgeRenderInfo();
        EdgeRenderInfo(const Handle& source, const Handle& target);
    };
    
    // Spatial partitioning
    Quadtree _quadtree;
    
    // Rendering state
    std::unordered_map<Handle, AtomRenderInfo> _atomRenderInfo;
    std::vector<EdgeRenderInfo> _edgeRenderInfo;
    ViewState _viewState;
    RenderConfig _renderConfig;
    
    // Selection and highlighting
    HandleSeq _selectedAtoms;
    HandleSeq _highlightedAtoms;
    
    // Performance metrics
    int _visibleAtomCount;
    int _visibleEdgeCount;
    int _drawCallCount;
    float _lastFrameTime;
    
    // Detail level
    DetailLevel _detailLevel;
    bool _autoDynamicDetail;
    
    // Custom coloring
    std::function<Color(const Handle&)> _nodeColorProvider;
    std::function<Color(const Handle&)> _edgeColorProvider;
    
    // Helper methods
    void updateQuadtree();
    void calculateVisibility();
    void updateDynamicDetailLevel();
    void renderNodes(int viewportWidth, int viewportHeight);
    void renderEdges(int viewportWidth, int viewportHeight);
    void renderLabels(int viewportWidth, int viewportHeight);
    Vector2 worldToScreen(const Vector2& worldPos, int viewportWidth, int viewportHeight) const;
    Vector2 screenToWorld(int screenX, int screenY, int viewportWidth, int viewportHeight) const;
    Color calculateAtomColor(const Handle& h) const;
    Vector2 calculateAtomPosition(const Handle& h);
    void batchRender(const std::vector<Quadtree::Node>& nodes, int viewportWidth, int viewportHeight);
    void simplifyEdges(float threshold);
};

} // namespace opencog

#endif // _OPENCOG_OPTIMIZED_GRAPH_RENDERER_H 