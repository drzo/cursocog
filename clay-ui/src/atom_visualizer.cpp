#include "../include/opencog_clay_bindings.h"
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cmath>
#include <map>
#include <set>

// Forward declarations
class AtomSpaceGraph;
class AtomNode;
class AtomLink;

// Constants for visualization
const float NODE_RADIUS = 25.0f;
const float LINK_THICKNESS = 2.0f;
const float SPRING_STRENGTH = 0.01f;
const float REPULSION_STRENGTH = 1000.0f;
const float DAMPING = 0.8f;
const float MIN_VELOCITY = 0.01f;
const int MAX_SIMULATION_STEPS = 100;

// Node colors by type
const std::map<std::string, Clay_Color> NODE_COLORS = {
    {"ConceptNode", {60, 180, 75, 255}},
    {"PredicateNode", {230, 25, 75, 255}},
    {"VariableNode", {255, 225, 25, 255}},
    {"NumberNode", {0, 130, 200, 255}},
    {"ListLink", {245, 130, 48, 255}},
    {"EvaluationLink", {70, 240, 240, 255}},
    {"InheritanceLink", {240, 50, 230, 255}},
    {"ExecutionLink", {210, 245, 60, 255}},
    {"DefaultNode", {128, 128, 128, 255}}
};

// Utility functions
Clay_Color GetNodeColor(const std::string& type) {
    auto it = NODE_COLORS.find(type);
    if (it != NODE_COLORS.end()) {
        return it->second;
    }
    return NODE_COLORS.at("DefaultNode");
}

// Graph physics simulation
class AtomSpaceGraph {
public:
    struct NodePosition {
        float x, y;
        float vx, y;
        bool fixed;
    };

    struct AtomNodeInfo {
        std::string id;
        std::string type;
        std::string name;
        float strength;
        float confidence;
        bool selected;
        NodePosition position;
    };

    struct AtomLinkInfo {
        std::string sourceId;
        std::string targetId;
        std::string type;
    };

    AtomSpaceGraph() : rng(std::random_device{}()) {}

    void AddNode(const std::string& id, const std::string& type, const std::string& name,
                float strength = 1.0f, float confidence = 1.0f) {
        if (nodes.find(id) != nodes.end()) {
            return; // Node already exists
        }

        AtomNodeInfo node;
        node.id = id;
        node.type = type;
        node.name = name;
        node.strength = strength;
        node.confidence = confidence;
        node.selected = false;
        
        // Random initial position
        std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
        node.position.x = dist(rng);
        node.position.y = dist(rng);
        node.position.vx = 0.0f;
        node.position.y = 0.0f;
        node.position.fixed = false;

        nodes[id] = node;
    }

    void AddLink(const std::string& sourceId, const std::string& targetId, const std::string& type) {
        if (nodes.find(sourceId) == nodes.end() || nodes.find(targetId) == nodes.end()) {
            return; // Source or target node doesn't exist
        }

        AtomLinkInfo link;
        link.sourceId = sourceId;
        link.targetId = targetId;
        link.type = type;

        links.push_back(link);
    }

    void SelectNode(const std::string& id) {
        if (nodes.find(id) != nodes.end()) {
            // Deselect all nodes first
            for (auto& pair : nodes) {
                pair.second.selected = false;
            }
            // Select the requested node
            nodes[id].selected = true;
        }
    }

    void FixNodePosition(const std::string& id, bool fixed) {
        if (nodes.find(id) != nodes.end()) {
            nodes[id].position.fixed = fixed;
        }
    }

    void RunSimulation(int steps = MAX_SIMULATION_STEPS) {
        for (int i = 0; i < steps; i++) {
            bool stable = UpdatePositions();
            if (stable) {
                break;
            }
        }
    }

    void Render(Clay_BoundingBox bounds) {
        // Render links first (underneath nodes)
        RenderLinks(bounds);
        // Then render nodes
        RenderNodes(bounds);
    }

private:
    std::map<std::string, AtomNodeInfo> nodes;
    std::vector<AtomLinkInfo> links;
    std::mt19937 rng;

    bool UpdatePositions() {
        bool stable = true;

        // Calculate forces
        std::map<std::string, std::pair<float, float>> forces;
        for (const auto& nodePair : nodes) {
            forces[nodePair.first] = {0.0f, 0.0f};
        }

        // Repulsive forces between all nodes
        for (const auto& node1Pair : nodes) {
            const auto& node1 = node1Pair.second;
            if (node1.position.fixed) continue;

            for (const auto& node2Pair : nodes) {
                if (node1Pair.first == node2Pair.first) continue;
                const auto& node2 = node2Pair.second;

                float dx = node1.position.x - node2.position.x;
                float dy = node1.position.y - node2.position.y;
                float distance = std::sqrt(dx * dx + dy * dy);
                if (distance < 0.1f) distance = 0.1f;

                float repulsion = REPULSION_STRENGTH / (distance * distance);
                forces[node1Pair.first].first += repulsion * dx / distance;
                forces[node1Pair.first].second += repulsion * dy / distance;
            }
        }

        // Spring forces along links
        for (const auto& link : links) {
            const auto& source = nodes[link.sourceId];
            const auto& target = nodes[link.targetId];

            float dx = source.position.x - target.position.x;
            float dy = source.position.y - target.position.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance < 0.1f) distance = 0.1f;

            float spring = -SPRING_STRENGTH * distance;
            
            if (!source.position.fixed) {
                forces[link.sourceId].first += spring * dx / distance;
                forces[link.sourceId].second += spring * dy / distance;
            }

            if (!target.position.fixed) {
                forces[link.targetId].first -= spring * dx / distance;
                forces[link.targetId].second -= spring * dy / distance;
            }
        }

        // Update positions based on forces
        for (auto& nodePair : nodes) {
            auto& node = nodePair.second;
            if (node.position.fixed) continue;

            float fx = forces[nodePair.first].first;
            float fy = forces[nodePair.first].second;

            // Apply forces to velocity
            node.position.vx = (node.position.vx + fx) * DAMPING;
            node.position.y = (node.position.y + fy) * DAMPING;

            // Update position
            node.position.x += node.position.vx;
            node.position.y += node.position.y;

            // Check if still moving significantly
            if (std::abs(node.position.vx) > MIN_VELOCITY || 
                std::abs(node.position.y) > MIN_VELOCITY) {
                stable = false;
            }
        }

        return stable;
    }

    void RenderNodes(Clay_BoundingBox bounds) {
        float centerX = bounds.x + bounds.width * 0.5f;
        float centerY = bounds.y + bounds.height * 0.5f;
        float scale = std::min(bounds.width, bounds.height) * 0.4f;

        for (const auto& nodePair : nodes) {
            const auto& node = nodePair.second;
            
            // Calculate screen position
            float screenX = centerX + node.position.x * scale / 200.0f;
            float screenY = centerY + node.position.y * scale / 200.0f;
            
            // Draw node circle
            Clay_Color color = GetNodeColor(node.type);
            if (node.selected) {
                // Highlight selected node
                color.r = std::min(color.r + 50, 255);
                color.g = std::min(color.g + 50, 255);
                color.b = std::min(color.b + 50, 255);
            }
            
            Clay_BoundingBox nodeBounds = {
                screenX - NODE_RADIUS,
                screenY - NODE_RADIUS,
                NODE_RADIUS * 2.0f,
                NODE_RADIUS * 2.0f
            };
            
            // Draw node (this would call into Clay UI)
            // For now, we'll assume there's a function to draw circles
            Clay_CornerRadius cornerRadius = {NODE_RADIUS, NODE_RADIUS, NODE_RADIUS, NODE_RADIUS};
            // This is a placeholder for the actual Clay UI render call
            // RenderCircle(nodeBounds, color);
            
            // Draw node label
            Clay_BoundingBox labelBounds = {
                screenX - NODE_RADIUS * 1.5f,
                screenY + NODE_RADIUS * 1.2f,
                NODE_RADIUS * 3.0f,
                NODE_RADIUS * 0.8f
            };
            
            // This is a placeholder for the actual Clay UI text render call
            // RenderText(labelBounds, node.name, white);
        }
    }

    void RenderLinks(Clay_BoundingBox bounds) {
        float centerX = bounds.x + bounds.width * 0.5f;
        float centerY = bounds.y + bounds.height * 0.5f;
        float scale = std::min(bounds.width, bounds.height) * 0.4f;

        for (const auto& link : links) {
            const auto& source = nodes[link.sourceId];
            const auto& target = nodes[link.targetId];
            
            // Calculate screen positions
            float sourceX = centerX + source.position.x * scale / 200.0f;
            float sourceY = centerY + source.position.y * scale / 200.0f;
            float targetX = centerX + target.position.x * scale / 200.0f;
            float targetY = centerY + target.position.y * scale / 200.0f;
            
            // Draw line
            Clay_Color linkColor = {150, 150, 150, 200};
            
            // This is a placeholder for the actual Clay UI line render call
            // RenderLine(sourceX, sourceY, targetX, targetY, linkColor, LINK_THICKNESS);
        }
    }
};

// Global instance for now
static AtomSpaceGraph g_atomSpaceGraph;

// Implementation of OpenCogClayBindings API functions
extern "C" {

void OpenCogClayBindings_RenderAtomSpaceGraph(Clay_BoundingBox boundingBox, void* atomSpacePtr) {
    // For now, we'll use our static graph
    // In the future, we would populate the graph from the actual AtomSpace
    g_atomSpaceGraph.Render(boundingBox);
}

void OpenCogClayBindings_SelectAtom(const char* atomId) {
    g_atomSpaceGraph.SelectNode(atomId);
}

void OpenCogClayBindings_FixAtomPosition(const char* atomId, bool fixed) {
    g_atomSpaceGraph.FixNodePosition(atomId, fixed);
}

void OpenCogClayBindings_RunGraphSimulation(int steps) {
    g_atomSpaceGraph.RunSimulation(steps);
}

// Demo function to populate the graph with some sample data
void OpenCogClayBindings_CreateDemoAtomSpace() {
    // Clear existing data
    g_atomSpaceGraph = AtomSpaceGraph();
    
    // Add some nodes
    g_atomSpaceGraph.AddNode("concept1", "ConceptNode", "Person");
    g_atomSpaceGraph.AddNode("concept2", "ConceptNode", "Animal");
    g_atomSpaceGraph.AddNode("concept3", "ConceptNode", "Plant");
    g_atomSpaceGraph.AddNode("predicate1", "PredicateNode", "likes");
    g_atomSpaceGraph.AddNode("predicate2", "PredicateNode", "eats");
    g_atomSpaceGraph.AddNode("list1", "ListLink", "list1");
    g_atomSpaceGraph.AddNode("list2", "ListLink", "list2");
    g_atomSpaceGraph.AddNode("eval1", "EvaluationLink", "eval1");
    g_atomSpaceGraph.AddNode("eval2", "EvaluationLink", "eval2");
    
    // Add some links
    g_atomSpaceGraph.AddLink("eval1", "predicate1", "EvaluationLink");
    g_atomSpaceGraph.AddLink("eval1", "list1", "EvaluationLink");
    g_atomSpaceGraph.AddLink("list1", "concept1", "ListLink");
    g_atomSpaceGraph.AddLink("list1", "concept2", "ListLink");
    g_atomSpaceGraph.AddLink("eval2", "predicate2", "EvaluationLink");
    g_atomSpaceGraph.AddLink("eval2", "list2", "EvaluationLink");
    g_atomSpaceGraph.AddLink("list2", "concept2", "ListLink");
    g_atomSpaceGraph.AddLink("list2", "concept3", "ListLink");
    
    // Run initial layout
    g_atomSpaceGraph.RunSimulation();
}

} // extern "C" 