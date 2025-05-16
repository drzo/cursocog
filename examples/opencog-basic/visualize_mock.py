#!/usr/bin/env python3
"""
AtomSpace Clay UI Mock Visualization
This script creates a mock visualization of what the Clay UI with AtomSpace would look like using Matplotlib
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.lines import Line2D
import networkx as nx
import json
from datetime import datetime
import random
import matplotlib

# Try to use TkAgg backend to make it interactive
try:
    matplotlib.use('TkAgg')
except:
    pass  # Fall back to default backend if TkAgg is not available

# Check for required packages
try:
    import matplotlib.pyplot as plt
    import networkx as nx
except ImportError:
    print("This script requires matplotlib and networkx.")
    print("Install them with: pip install matplotlib networkx")
    sys.exit(1)

class MockAtomSpace:
    """A simple class to simulate an AtomSpace for visualization purposes"""
    
    def __init__(self):
        self.nodes = {}
        self.links = []
        self.types = {
            "ConceptNode": "#3498db",  # Blue
            "PredicateNode": "#e74c3c",  # Red
            "InheritanceLink": "#2ecc71",  # Green
            "EvaluationLink": "#f39c12",  # Orange
            "ListLink": "#9b59b6"  # Purple
        }
    
    def add_node(self, name, node_type, truth_value=None):
        """Add a node to the mock AtomSpace"""
        self.nodes[name] = {
            "name": name,
            "type": node_type,
            "truth_value": truth_value or [0.0, 0.0],
            "position": [random.uniform(-10, 10), random.uniform(-10, 10), random.uniform(-5, 5)]
        }
        return name
    
    def add_link(self, link_type, outgoing_set, truth_value=None):
        """Add a link to the mock AtomSpace"""
        link_id = f"{link_type}_{len(self.links)}"
        self.links.append({
            "id": link_id,
            "type": link_type,
            "outgoing_set": outgoing_set,
            "truth_value": truth_value or [0.0, 0.0]
        })
        return link_id
    
    def to_networkx(self):
        """Convert the mock AtomSpace to a NetworkX graph for visualization"""
        G = nx.Graph()
        
        # Add all nodes
        for name, node_data in self.nodes.items():
            G.add_node(name, 
                       type=node_data["type"], 
                       truth_value=node_data["truth_value"],
                       position=node_data["position"],
                       color=self.types.get(node_data["type"], "#aaaaaa"))
        
        # Add all links
        for link in self.links:
            # For visualization, we'll create edges between each pair of nodes in the outgoing set
            outgoing = link["outgoing_set"]
            if len(outgoing) >= 2:
                for i in range(len(outgoing) - 1):
                    G.add_edge(outgoing[i], outgoing[i+1], 
                               type=link["type"],
                               truth_value=link["truth_value"],
                               color=self.types.get(link["type"], "#aaaaaa"))
        
        return G

def create_sample_atomspace():
    """Create a sample AtomSpace with various types of nodes and links"""
    atomspace = MockAtomSpace()
    
    # Create concept nodes
    human = atomspace.add_node("human", "ConceptNode", [0.9, 0.8])
    woman = atomspace.add_node("woman", "ConceptNode", [0.9, 0.9])
    man = atomspace.add_node("man", "ConceptNode", [0.9, 0.9])
    mammal = atomspace.add_node("mammal", "ConceptNode", [0.9, 0.9])
    animal = atomspace.add_node("animal", "ConceptNode", [0.9, 0.9])
    alice = atomspace.add_node("Alice", "ConceptNode", [1.0, 1.0])
    bob = atomspace.add_node("Bob", "ConceptNode", [1.0, 1.0])
    fido = atomspace.add_node("Fido", "ConceptNode", [1.0, 1.0])
    
    # Create predicates
    likes = atomspace.add_node("likes", "PredicateNode", [1.0, 1.0])
    
    # Create inheritance links
    atomspace.add_link("InheritanceLink", [woman, human], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [man, human], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [human, mammal], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [mammal, animal], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [alice, woman], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [bob, man], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [fido, mammal], [1.0, 1.0])
    
    # Create lists
    list1 = atomspace.add_link("ListLink", [alice, bob], [1.0, 1.0])
    list2 = atomspace.add_link("ListLink", [bob, alice], [1.0, 1.0])
    list3 = atomspace.add_link("ListLink", [alice, fido], [1.0, 1.0])
    
    # Create evaluation links
    atomspace.add_link("EvaluationLink", [likes, list1], [1.0, 1.0])
    atomspace.add_link("EvaluationLink", [likes, list2], [1.0, 1.0])
    atomspace.add_link("EvaluationLink", [likes, list3], [1.0, 1.0])
    
    return atomspace

def visualize_atomspace(atomspace, title="Clay UI - AtomSpace Visualization"):
    """Visualize the AtomSpace using matplotlib"""
    G = atomspace.to_networkx()
    
    # Create a figure with a cool dark theme
    fig = plt.figure(figsize=(12, 10), facecolor='#1e1e1e')
    ax = fig.add_subplot(111, facecolor='#1e1e1e')
    ax.set_title(title, color='white', fontsize=16)
    ax.set_axis_off()
    
    # Create a layout for the graph - spring layout works well
    pos = nx.spring_layout(G, k=0.15, iterations=50)
    
    # Get node colors from the graph attributes
    node_colors = [G.nodes[n]['color'] for n in G.nodes()]
    
    # Draw nodes
    nx.draw_networkx_nodes(G, pos, node_size=700, node_color=node_colors, alpha=0.9, edgecolors='white')
    
    # Draw edges
    edge_colors = [G.edges[e]['color'] for e in G.edges()]
    nx.draw_networkx_edges(G, pos, width=2, edge_color=edge_colors, alpha=0.7)
    
    # Draw node labels
    nx.draw_networkx_labels(G, pos, font_size=10, font_family='sans-serif', font_weight='bold', font_color='white')
    
    # Add a legend for atom types
    legend_elements = []
    for atom_type, color in atomspace.types.items():
        legend_elements.append(Line2D([0], [0], marker='o', color='w', label=atom_type,
                                      markerfacecolor=color, markersize=10))
    
    ax.legend(handles=legend_elements, loc='upper right', facecolor='#2d2d2d', edgecolor='gray', fontsize=10)
    
    # Add mock Clay UI interface elements
    add_mock_ui(fig, ax)
    
    # Add truth value display for a selected node
    selected_node = "Alice"
    if selected_node in G.nodes:
        add_node_details(fig, ax, selected_node, G.nodes[selected_node], pos[selected_node])
    
    # Adjust the layout and show
    plt.tight_layout()
    plt.subplots_adjust(left=0.2)
    
    return fig, ax

def add_mock_ui(fig, ax):
    """Add mock Clay UI interface elements to the visualization"""
    # Add a sidebar
    sidebar = plt.Rectangle((-0.1, 0), 0.3, 1, transform=fig.transFigure, 
                           facecolor='#2d2d2d', edgecolor='none', alpha=0.9)
    fig.patches.append(sidebar)
    
    # Add a title to the sidebar
    fig.text(0.05, 0.95, "Clay UI", fontsize=14, color='white', fontweight='bold')
    
    # Add control sections
    sections = [
        ("File", 0.90),
        ("View", 0.85),
        ("Layout", 0.80),
        ("Filter", 0.75),
        ("Search", 0.70),
        ("Settings", 0.65),
        ("Help", 0.60),
    ]
    
    for name, y_pos in sections:
        fig.text(0.05, y_pos, name, fontsize=12, color='white')
    
    # Add a search box
    search_box = plt.Rectangle((0.03, 0.67), 0.24, 0.02, transform=fig.transFigure,
                               facecolor='#3d3d3d', edgecolor='#555555')
    fig.patches.append(search_box)
    fig.text(0.04, 0.675, "Search atoms...", fontsize=10, color='#aaaaaa')
    
    # Add filter options
    filter_y = 0.55
    fig.text(0.05, filter_y, "Filter by Type:", fontsize=10, color='white')
    filter_y -= 0.03
    
    for atom_type, color in list(create_sample_atomspace().types.items())[:4]:
        checkbox = plt.Rectangle((0.05, filter_y-0.005), 0.01, 0.01, transform=fig.transFigure,
                                 facecolor=color, edgecolor='white')
        fig.patches.append(checkbox)
        fig.text(0.07, filter_y, atom_type, fontsize=9, color='white')
        filter_y -= 0.02
    
    # Add a status bar
    status_bar = plt.Rectangle((0, 0), 1, 0.03, transform=fig.transFigure,
                               facecolor='#2d2d2d', edgecolor='none')
    fig.patches.append(status_bar)
    
    # Add status info
    current_time = datetime.now().strftime("%H:%M:%S")
    fig.text(0.01, 0.015, f"Ready | Atoms: 19 | Links: 12 | {current_time}", 
             fontsize=9, color='#aaaaaa')
    
    # Add a toolbar at the top
    toolbar = plt.Rectangle((0.3, 0.97), 0.7, 0.03, transform=fig.transFigure,
                           facecolor='#2d2d2d', edgecolor='none')
    fig.patches.append(toolbar)
    
    # Add toolbar buttons
    toolbar_items = ["Connect", "Open", "Save", "Export", "Layout", "Reset", "Zoom In", "Zoom Out"]
    for i, item in enumerate(toolbar_items):
        x_pos = 0.32 + i * 0.08
        fig.text(x_pos, 0.978, item, fontsize=9, color='white')

def add_node_details(fig, ax, node_name, node_data, position):
    """Add a details panel for a selected node"""
    # Calculate position for the details box in figure coordinates
    fig_pos = ax.transData.transform(position)
    fig_pos = fig.transFigure.inverted().transform(fig_pos)
    
    # Add a details panel
    details_box = plt.Rectangle((fig_pos[0] + 0.05, fig_pos[1] - 0.1), 0.25, 0.2, 
                               transform=fig.transFigure, facecolor='#2d2d2d', 
                               edgecolor='white', alpha=0.9, zorder=5)
    fig.patches.append(details_box)
    
    # Add a title with node name
    fig.text(fig_pos[0] + 0.06, fig_pos[1] + 0.08, node_name, 
             fontsize=12, color='white', fontweight='bold', zorder=6)
    
    # Add details
    details = [
        f"Type: {node_data['type']}",
        f"TruthValue: {node_data['truth_value'][0]:.2f}, {node_data['truth_value'][1]:.2f}",
        f"Position: {node_data['position'][0]:.2f}, {node_data['position'][1]:.2f}, {node_data['position'][2]:.2f}",
        f"Incoming Links: 3",
        f"Outgoing Links: 2"
    ]
    
    for i, detail in enumerate(details):
        fig.text(fig_pos[0] + 0.06, fig_pos[1] + 0.06 - i * 0.02, 
                 detail, fontsize=9, color='white', zorder=6)
    
    # Add a close button
    close_button = plt.Rectangle((fig_pos[0] + 0.28, fig_pos[1] + 0.08), 0.015, 0.015, 
                                transform=fig.transFigure, facecolor='#e74c3c', 
                                edgecolor='white', zorder=6)
    fig.patches.append(close_button)
    fig.text(fig_pos[0] + 0.283, fig_pos[1] + 0.079, "×", 
             fontsize=10, color='white', zorder=7)

def main():
    """Main function to demonstrate AtomSpace visualization"""
    print("OpenCog Clay UI AtomSpace Visualization Mock")
    print("===========================================")
    
    # Create a sample AtomSpace
    atomspace = create_sample_atomspace()
    
    # Visualize the AtomSpace
    fig, ax = visualize_atomspace(atomspace)
    
    print("\nDisplaying mock visualization of Clay UI with AtomSpace")
    print("This is a simplified representation of what the actual Clay UI looks like")
    print("The real Clay UI has more advanced features and 3D visualization capabilities")
    
    # Show the plot
    plt.show()

if __name__ == "__main__":
    main() 