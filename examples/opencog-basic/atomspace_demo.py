#!/usr/bin/env python3
"""
OpenCog Basic AtomSpace Demo
This script demonstrates how to create a simple AtomSpace and visualize it with Clay UI
"""

import os
import sys
import time
import subprocess

# Try to import opencog, handling potential path issues
try:
    from opencog.atomspace import AtomSpace, TruthValue
    from opencog.atomspace import types 
    from opencog.type_constructors import ConceptNode, PredicateNode, LinkValue
    from opencog.type_constructors import Inheritance, Evaluation, ListLink
except ImportError:
    print("Error: opencog modules not found. Please ensure OpenCog Basic is properly installed.")
    print("Follow the installation instructions at: https://github.com/opencog/opencog-basic")
    sys.exit(1)

def create_demo_atomspace():
    """Create a demo AtomSpace with some sample atoms"""
    atomspace = AtomSpace()
    
    # Need to set the AtomSpace for the type constructors
    from opencog.utilities import initialize_opencog
    initialize_opencog(atomspace)
    
    # Create concept nodes for humans and animals
    human = ConceptNode("human")
    human.truth_value(0.9, 0.8)  # Set TruthValue (strength, confidence)
    
    woman = ConceptNode("woman")
    woman.truth_value(0.9, 0.9)
    
    man = ConceptNode("man")
    man.truth_value(0.9, 0.9)
    
    mammal = ConceptNode("mammal")
    mammal.truth_value(0.9, 0.9)
    
    animal = ConceptNode("animal")
    animal.truth_value(0.9, 0.9)
    
    # Create some named individuals
    alice = ConceptNode("Alice")
    alice.truth_value(1.0, 1.0)
    
    bob = ConceptNode("Bob")
    bob.truth_value(1.0, 1.0)
    
    fido = ConceptNode("Fido")
    fido.truth_value(1.0, 1.0)
    
    # Create predicates
    likes = PredicateNode("likes")
    likes.truth_value(1.0, 1.0)
    
    # Create inheritance links (representing "is-a" relationships)
    # Woman is a Human
    Inheritance(woman, human)
    
    # Man is a Human
    Inheritance(man, human)
    
    # Human is a Mammal
    Inheritance(human, mammal)
    
    # Mammal is an Animal
    Inheritance(mammal, animal)
    
    # Alice is a Woman
    Inheritance(alice, woman)
    
    # Bob is a Man
    Inheritance(bob, man)
    
    # Fido is a Mammal
    Inheritance(fido, mammal)
    
    # Create some evaluations (representing predicates)
    # Alice likes Bob
    Evaluation(likes, ListLink(alice, bob))
    
    # Bob likes Alice
    Evaluation(likes, ListLink(bob, alice))
    
    # Alice likes Fido 
    Evaluation(likes, ListLink(alice, fido))
    
    print("Created AtomSpace with sample atoms:")
    print(f"Total atom count: {len(atomspace)}")
    
    return atomspace

def start_clay_ui_visualization(atomspace):
    """Launch the Clay UI visualization for the AtomSpace"""
    try:
        # In a real implementation, this would call the Clay UI directly
        # For this demo, we'll just print instructions
        print("\nTo visualize the AtomSpace with Clay UI:")
        print("1. Run: cd clay-ui/build")
        print("2. Run: ./opencog-atomspace-viewer")
        print("\nIf the Clay UI is properly installed, a window will open with the AtomSpace visualization.")
        print("You'll be able to interact with the graph, view atom details, and explore the relationships.")
        
        # This would be the actual implementation if Clay UI was installed:
        # viewer_path = os.path.join(os.getcwd(), "..", "clay-ui", "build", "opencog-atomspace-viewer")
        # if os.path.exists(viewer_path):
        #     subprocess.Popen([viewer_path])
        # else:
        #     print(f"Clay UI viewer not found at {viewer_path}")
    except Exception as e:
        print(f"Error launching Clay UI: {e}")

def main():
    """Main function to run the demo"""
    print("OpenCog Basic AtomSpace Demo")
    print("============================")
    
    # Create a sample AtomSpace
    atomspace = create_demo_atomspace()
    
    # Display the AtomSpace content (simplified)
    print("\nAtomSpace contents:")
    print("-------------------")
    for atom in atomspace:
        print(f"- {atom}")
    
    # Launch Clay UI for visualization
    start_clay_ui_visualization(atomspace)
    
    # Keep the script running to maintain the AtomSpace
    print("\nPress Ctrl+C to exit...")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nExiting AtomSpace demo")

if __name__ == "__main__":
    main() 