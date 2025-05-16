/*
 * real_time_visualization_demo.cpp
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

/**
 * Real-Time AtomSpace Visualization Demo
 *
 * This program demonstrates the real-time visualization of an AtomSpace.
 * It creates an AtomSpace, populates it with some sample data, and then
 * continuously adds, modifies, and removes atoms while visualizing the changes.
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <string>
#include <memory>

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/truthvalue/SimpleTruthValue.h>
#include <opencog/RealTimeAtomSpaceVisualizer.h>

// For the UI framework
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace opencog;
using namespace std::chrono;

// Random number generation
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> disFloat(0.0, 1.0);
std::uniform_int_distribution<> disInt(1, 10);

// Forward declaration of utility functions
void createRandomAtomNetwork(AtomSpacePtr atomspace, int numNodes, int numLinks);
std::string getRandomWord();
void setupImGui(GLFWwindow* window);
void renderUI(RealTimeAtomSpaceVisualizer& visualizer);
void renderAtomSpaceStatistics(AtomSpacePtr atomspace, RealTimeAtomSpaceVisualizer& visualizer);
void performRandomAtomSpaceChanges(AtomSpacePtr atomspace);

int main(int argc, char** argv)
{
    std::cout << "Real-Time AtomSpace Visualization Demo" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    // Initialize GLFW and create window
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(1280, 800, "OpenCog Real-Time AtomSpace Visualizer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // Initialize ImGui
    setupImGui(window);
    
    // Create AtomSpace
    AtomSpacePtr atomspace = createAtomSpace();
    
    // Create initial random atom network
    createRandomAtomNetwork(atomspace, 50, 75);
    
    // Create visualizer and connect to AtomSpace
    RealTimeAtomSpaceVisualizer visualizer;
    visualizer.connectToAtomSpace(atomspace);
    visualizer.setLayoutMode(RealTimeAtomSpaceVisualizer::LayoutMode::FORCE_DIRECTED);
    visualizer.setMaxVisibleNodes(200);
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Render UI
        renderUI(visualizer);
        
        // Display AtomSpace statistics
        renderAtomSpaceStatistics(atomspace, visualizer);
        
        // Randomly modify AtomSpace occasionally
        if (disFloat(gen) < 0.05) {
            performRandomAtomSpaceChanges(atomspace);
        }
        
        // Render ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        
        // Wait a bit to limit frame rate
        std::this_thread::sleep_for(milliseconds(16));
    }
    
    // Cleanup
    visualizer.disconnectFromAtomSpace();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}

void createRandomAtomNetwork(AtomSpacePtr atomspace, int numNodes, int numLinks)
{
    std::cout << "Creating random atom network with " << numNodes << " nodes and " << numLinks << " links" << std::endl;
    
    // Create nodes
    std::vector<Handle> nodes;
    for (int i = 0; i < numNodes; i++) {
        // Choose random node type (ConceptNode, PredicateNode, etc.)
        Type nodeType;
        int typeRoll = disInt(gen) % 3;
        
        switch (typeRoll) {
            case 0: nodeType = CONCEPT_NODE; break;
            case 1: nodeType = PREDICATE_NODE; break;
            default: nodeType = VARIABLE_NODE; break;
        }
        
        // Create node with a random name
        std::string name = getRandomWord();
        Handle h = atomspace->add_node(nodeType, name);
        
        // Set a random truth value
        float strength = disFloat(gen);
        float confidence = disFloat(gen) * 0.9f + 0.1f; // Ensure some minimum confidence
        TruthValuePtr tv = SimpleTruthValue::createTV(strength, confidence);
        h->setTruthValue(tv);
        
        nodes.push_back(h);
    }
    
    // Create links between random nodes
    for (int i = 0; i < numLinks; i++) {
        // Choose random link type
        Type linkType;
        int typeRoll = disInt(gen) % 4;
        
        switch (typeRoll) {
            case 0: linkType = LIST_LINK; break;
            case 1: linkType = EVALUATION_LINK; break;
            case 2: linkType = INHERITANCE_LINK; break;
            default: linkType = SET_LINK; break;
        }
        
        // Choose random number of targets (arity)
        int arity = (disInt(gen) % 3) + 2; // 2-4 atoms per link
        HandleSeq outgoing;
        
        for (int j = 0; j < arity; j++) {
            int nodeIndex = disInt(gen) % nodes.size();
            outgoing.push_back(nodes[nodeIndex]);
        }
        
        // Create link
        Handle h = atomspace->add_link(linkType, outgoing);
        
        // Set a random truth value
        float strength = disFloat(gen);
        float confidence = disFloat(gen) * 0.9f + 0.1f;
        TruthValuePtr tv = SimpleTruthValue::createTV(strength, confidence);
        h->setTruthValue(tv);
    }
}

std::string getRandomWord()
{
    // List of sample words to use for node names
    static const std::vector<std::string> words = {
        "apple", "banana", "car", "dog", "elephant", "fruit", "guitar", "house",
        "ice", "jacket", "key", "lamp", "music", "notebook", "orange", "pencil",
        "queen", "rabbit", "sun", "tree", "umbrella", "violin", "water", "xylophone",
        "yacht", "zebra", "atom", "brain", "cognition", "data", "energy", "force"
    };
    
    return words[disInt(gen) % words.size()] + std::to_string(disInt(gen));
}

void setupImGui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void renderUI(RealTimeAtomSpaceVisualizer& visualizer)
{
    ImGui::Begin("Controls");
    
    // Layout mode selection
    static int layoutMode = static_cast<int>(visualizer.getLayoutMode());
    const char* layoutModes[] = {"Force-Directed", "Hierarchical", "Radial", "Grid"};
    if (ImGui::Combo("Layout Mode", &layoutMode, layoutModes, IM_ARRAYSIZE(layoutModes))) {
        visualizer.setLayoutMode(static_cast<RealTimeAtomSpaceVisualizer::LayoutMode>(layoutMode));
    }
    
    // Color mode selection
    static int colorMode = static_cast<int>(visualizer.getColorMode());
    const char* colorModes[] = {"Type-Based", "Truth Value", "Attention Value", "Custom"};
    if (ImGui::Combo("Color Mode", &colorMode, colorModes, IM_ARRAYSIZE(colorModes))) {
        visualizer.setColorMode(static_cast<RealTimeAtomSpaceVisualizer::ColorMode>(colorMode));
    }
    
    // Max visible nodes slider
    static int maxNodes = visualizer.getMaxVisibleNodes();
    if (ImGui::SliderInt("Max Visible Nodes", &maxNodes, 10, 500)) {
        visualizer.setMaxVisibleNodes(maxNodes);
    }
    
    // Truth value filter controls
    static float minConfidence = 0.0f;
    static float minStrength = 0.0f;
    bool filterChanged = false;
    
    filterChanged |= ImGui::SliderFloat("Min. Confidence", &minConfidence, 0.0f, 1.0f);
    filterChanged |= ImGui::SliderFloat("Min. Strength", &minStrength, 0.0f, 1.0f);
    
    if (filterChanged) {
        visualizer.setTruthValueFilter(minConfidence, minStrength);
    }
    
    if (ImGui::Button("Clear Filters")) {
        minConfidence = 0.0f;
        minStrength = 0.0f;
        visualizer.clearTruthValueFilter();
        visualizer.clearTypeFilter();
    }
    
    ImGui::Separator();
    
    // Camera controls
    if (ImGui::Button("Zoom to Fit")) {
        visualizer.zoomToFit();
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Zoom In")) {
        visualizer.zoomIn();
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Zoom Out")) {
        visualizer.zoomOut();
    }
    
    ImGui::Separator();
    
    // Export options
    if (ImGui::Button("Export to PNG")) {
        visualizer.exportToPNG("atomspace_visualization.png");
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Export to SVG")) {
        visualizer.exportToSVG("atomspace_visualization.svg");
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Export to JSON")) {
        visualizer.exportToJSON("atomspace_visualization.json");
    }
    
    ImGui::End();
}

void renderAtomSpaceStatistics(AtomSpacePtr atomspace, RealTimeAtomSpaceVisualizer& visualizer)
{
    ImGui::Begin("AtomSpace Statistics");
    
    ImGui::Text("Total Nodes: %zu", visualizer.getTotalNodeCount());
    ImGui::Text("Total Links: %zu", visualizer.getTotalLinkCount());
    ImGui::Separator();
    ImGui::Text("Visible Nodes: %zu", visualizer.getVisibleNodeCount());
    ImGui::Text("Visible Links: %zu", visualizer.getVisibleLinkCount());
    ImGui::Separator();
    ImGui::Text("Pending Events: %zu", visualizer.getPendingEventCount());
    
    ImGui::End();
}

void performRandomAtomSpaceChanges(AtomSpacePtr atomspace)
{
    // Decide what kind of change to make
    int changeType = disInt(gen) % 3;
    
    switch (changeType) {
        case 0: // Add a new node
            {
                Type nodeType;
                int typeRoll = disInt(gen) % 3;
                
                switch (typeRoll) {
                    case 0: nodeType = CONCEPT_NODE; break;
                    case 1: nodeType = PREDICATE_NODE; break;
                    default: nodeType = VARIABLE_NODE; break;
                }
                
                std::string name = getRandomWord();
                Handle h = atomspace->add_node(nodeType, name);
                
                float strength = disFloat(gen);
                float confidence = disFloat(gen) * 0.9f + 0.1f;
                TruthValuePtr tv = SimpleTruthValue::createTV(strength, confidence);
                h->setTruthValue(tv);
            }
            break;
            
        case 1: // Modify an existing atom's TV
            {
                // Get a random atom
                HandleSeq allAtoms;
                atomspace->get_handles_by_type(allAtoms, ATOM, true);
                
                if (!allAtoms.empty()) {
                    int atomIndex = disInt(gen) % allAtoms.size();
                    Handle h = allAtoms[atomIndex];
                    
                    float strength = disFloat(gen);
                    float confidence = disFloat(gen) * 0.9f + 0.1f;
                    TruthValuePtr tv = SimpleTruthValue::createTV(strength, confidence);
                    h->setTruthValue(tv);
                }
            }
            break;
            
        case 2: // Add a new link between existing nodes
            {
                // Get all nodes
                HandleSeq allNodes;
                atomspace->get_handles_by_type(allNodes, NODE, true);
                
                if (allNodes.size() >= 2) {
                    // Choose random link type
                    Type linkType;
                    int typeRoll = disInt(gen) % 4;
                    
                    switch (typeRoll) {
                        case 0: linkType = LIST_LINK; break;
                        case 1: linkType = EVALUATION_LINK; break;
                        case 2: linkType = INHERITANCE_LINK; break;
                        default: linkType = SET_LINK; break;
                    }
                    
                    // Choose random number of targets
                    int arity = (disInt(gen) % 3) + 2; // 2-4 atoms per link
                    HandleSeq outgoing;
                    
                    for (int j = 0; j < arity; j++) {
                        int nodeIndex = disInt(gen) % allNodes.size();
                        outgoing.push_back(allNodes[nodeIndex]);
                    }
                    
                    // Create link
                    Handle h = atomspace->add_link(linkType, outgoing);
                    
                    float strength = disFloat(gen);
                    float confidence = disFloat(gen) * 0.9f + 0.1f;
                    TruthValuePtr tv = SimpleTruthValue::createTV(strength, confidence);
                    h->setTruthValue(tv);
                }
            }
            break;
    }
} 