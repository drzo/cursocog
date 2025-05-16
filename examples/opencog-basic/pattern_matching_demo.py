#!/usr/bin/env python3
"""
OpenCog Basic Pattern Matching Demo
This script demonstrates pattern matching in AtomSpace and visualization with Clay UI
"""

import os
import sys
import time

# Try to import opencog modules
try:
    from opencog.atomspace import AtomSpace, TruthValue, PtrValue
    from opencog.atomspace import types, create_child_atomspace
    from opencog.type_constructors import *
    from opencog.utilities import initialize_opencog
    from opencog.bindlink import execute_atom, satisfy_link, bindlink
except ImportError:
    print("Error: opencog modules not found. Please ensure OpenCog Basic is properly installed.")
    print("Follow the installation instructions at: https://github.com/opencog/opencog-basic")
    sys.exit(1)

def create_knowledge_base():
    """Create a knowledge base with facts about animals and their properties"""
    atomspace = AtomSpace()
    initialize_opencog(atomspace)
    
    # Define concepts
    animal = ConceptNode("animal")
    mammal = ConceptNode("mammal")
    bird = ConceptNode("bird")
    reptile = ConceptNode("reptile")
    
    # Define specific animals
    dog = ConceptNode("dog")
    cat = ConceptNode("cat")
    eagle = ConceptNode("eagle")
    sparrow = ConceptNode("sparrow")
    snake = ConceptNode("snake")
    lizard = ConceptNode("lizard")
    
    # Define properties
    can_fly = PredicateNode("can_fly")
    has_fur = PredicateNode("has_fur")
    lays_eggs = PredicateNode("lays_eggs")
    is_predator = PredicateNode("is_predator")
    has_scales = PredicateNode("has_scales")
    
    # Define hierarchical relationships
    Inheritance(mammal, animal)
    Inheritance(bird, animal)
    Inheritance(reptile, animal)
    
    Inheritance(dog, mammal)
    Inheritance(cat, mammal)
    Inheritance(eagle, bird)
    Inheritance(sparrow, bird)
    Inheritance(snake, reptile)
    Inheritance(lizard, reptile)
    
    # Define property relationships
    Evaluation(has_fur, ListLink(dog))
    Evaluation(has_fur, ListLink(cat))
    
    Evaluation(can_fly, ListLink(eagle))
    Evaluation(can_fly, ListLink(sparrow))
    
    Evaluation(lays_eggs, ListLink(bird))
    Evaluation(lays_eggs, ListLink(reptile))
    
    Evaluation(is_predator, ListLink(cat))
    Evaluation(is_predator, ListLink(eagle))
    Evaluation(is_predator, ListLink(snake))
    
    Evaluation(has_scales, ListLink(snake))
    Evaluation(has_scales, ListLink(lizard))
    
    print(f"Created knowledge base with {len(atomspace)} atoms")
    return atomspace

def pattern_match_flying_animals(atomspace):
    """Pattern match to find all animals that can fly"""
    print("\nSearching for animals that can fly...")
    
    # Define the pattern - find X where:
    # X is an animal
    # X can fly
    
    # Variables declaration
    variable_node = VariableNode("$X")
    variable_declaration = TypedVariableLink(
        variable_node,
        TypeNode("ConceptNode")
    )
    
    # Pattern to match
    animal_pattern = InheritanceLink(
        variable_node,
        ConceptNode("animal")
    )
    
    fly_pattern = EvaluationLink(
        PredicateNode("can_fly"),
        ListLink(variable_node)
    )
    
    # Combine patterns
    pattern = AndLink(
        animal_pattern,
        fly_pattern
    )
    
    # Create the bind link
    bind_link = BindLink(
        VariableList(variable_declaration),
        pattern,
        variable_node  # This is what we want returned
    )
    
    # Execute the pattern matching
    result_set = bindlink(atomspace, bind_link)
    
    # Print results
    for i, result in enumerate(result_set.out):
        print(f"Result {i+1}: {result}")
    
    return result_set

def pattern_match_furry_predators(atomspace):
    """Pattern match to find all animals that have fur and are predators"""
    print("\nSearching for animals that have fur and are predators...")
    
    # Variables declaration
    variable_node = VariableNode("$X")
    variable_declaration = TypedVariableLink(
        variable_node,
        TypeNode("ConceptNode")
    )
    
    # Pattern to match
    animal_pattern = InheritanceLink(
        variable_node,
        ConceptNode("animal")
    )
    
    fur_pattern = EvaluationLink(
        PredicateNode("has_fur"),
        ListLink(variable_node)
    )
    
    predator_pattern = EvaluationLink(
        PredicateNode("is_predator"),
        ListLink(variable_node)
    )
    
    # Combine patterns
    pattern = AndLink(
        animal_pattern,
        fur_pattern,
        predator_pattern
    )
    
    # Create the bind link
    bind_link = BindLink(
        VariableList(variable_declaration),
        pattern,
        variable_node
    )
    
    # Execute the pattern matching
    result_set = bindlink(atomspace, bind_link)
    
    # Print results
    for i, result in enumerate(result_set.out):
        print(f"Result {i+1}: {result}")
    
    return result_set

def infer_lays_eggs(atomspace):
    """Infer which animals lay eggs based on their type"""
    print("\nInferring which specific animals lay eggs...")
    
    # This query will find all specific animals that are birds or reptiles
    # since we have the relationship that birds and reptiles lay eggs
    
    # Variables declaration
    variable_x = VariableNode("$X")  # Specific animal
    variable_y = VariableNode("$Y")  # Category (bird or reptile)
    
    variable_declaration = VariableList(
        TypedVariableLink(variable_x, TypeNode("ConceptNode")),
        TypedVariableLink(variable_y, TypeNode("ConceptNode"))
    )
    
    # Pattern to match
    inheritance_pattern1 = InheritanceLink(
        variable_x,
        variable_y
    )
    
    lays_eggs_pattern = EvaluationLink(
        PredicateNode("lays_eggs"),
        ListLink(variable_y)
    )
    
    # Combine patterns
    pattern = AndLink(
        inheritance_pattern1,
        lays_eggs_pattern
    )
    
    # What we want returned - the specific animal and its category
    results_pattern = ListLink(
        variable_x,
        variable_y
    )
    
    # Create the bind link
    bind_link = BindLink(
        variable_declaration,
        pattern,
        results_pattern
    )
    
    # Execute the pattern matching
    result_set = bindlink(atomspace, bind_link)
    
    # Print results
    print("Animals that lay eggs:")
    for i, result in enumerate(result_set.out):
        animal = result.out[0]
        category = result.out[1]
        print(f"  {animal} (a {category})")
    
    return result_set

def launch_clay_ui():
    """Show instructions for launching Clay UI to visualize the AtomSpace"""
    print("\nTo visualize the AtomSpace with Clay UI:")
    print("1. Run: cd clay-ui/build")
    print("2. Run: ./opencog-atomspace-viewer")
    print("\nThe viewer will enable you to see the pattern matches graphically.")
    print("Try the following visualization tips:")
    print("- Use the search box to highlight 'can_fly' or 'is_predator'")
    print("- Filter by AtomType to see only PredicateNodes or ConceptNodes")
    print("- Use the 'Expand Node' feature on animal nodes to see their connections")

def main():
    """Main function demonstrating AtomSpace pattern matching"""
    print("OpenCog Basic Pattern Matching Demo")
    print("===================================")
    
    # Create a knowledge base
    atomspace = create_knowledge_base()
    
    # Perform pattern matching
    flying_animals = pattern_match_flying_animals(atomspace)
    furry_predators = pattern_match_furry_predators(atomspace)
    egg_layers = infer_lays_eggs(atomspace)
    
    # Show instructions for Clay UI visualization
    launch_clay_ui()
    
    # Keep the program running
    print("\nPress Ctrl+C to exit...")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nExiting pattern matching demo")

if __name__ == "__main__":
    main() 