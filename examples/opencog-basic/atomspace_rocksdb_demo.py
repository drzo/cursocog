#!/usr/bin/env python3
"""
OpenCog Basic AtomSpace with RocksDB Persistence Demo
This script demonstrates how AtomSpace works with RocksDB for persistence 
and how the Pattern Matching Engine can query stored data.
"""

import os
import sys
import time
import json
import random
import tempfile
import shutil
from datetime import datetime

class MockRocksDBStore:
    """Mock implementation of RocksDB store for AtomSpace"""
    def __init__(self, db_path=None):
        """Initialize the mock store"""
        if db_path is None:
            self.db_path = tempfile.mkdtemp(prefix="atomspace_rocksdb_")
        else:
            self.db_path = db_path
            os.makedirs(self.db_path, exist_ok=True)
        
        self.storage = {}
        self.load_from_disk()
        print(f"RocksDB store initialized at: {self.db_path}")
    
    def load_from_disk(self):
        """Load data from disk if available"""
        try:
            db_file = os.path.join(self.db_path, "atomspace.json")
            if os.path.exists(db_file):
                with open(db_file, 'r') as f:
                    self.storage = json.load(f)
                print(f"Loaded {len(self.storage)} atoms from disk")
            else:
                print("No existing database found, starting with empty AtomSpace")
        except Exception as e:
            print(f"Error loading from disk: {e}")
    
    def save_to_disk(self):
        """Save data to disk"""
        try:
            db_file = os.path.join(self.db_path, "atomspace.json")
            with open(db_file, 'w') as f:
                json.dump(self.storage, f, indent=2)
            print(f"Saved {len(self.storage)} atoms to disk")
        except Exception as e:
            print(f"Error saving to disk: {e}")
    
    def store_atom(self, atom_type, atom_name, truth_value=None, outgoing_set=None):
        """Store an atom in the database"""
        atom_id = self._generate_atom_id(atom_type, atom_name, outgoing_set)
        
        # Create atom data structure
        atom_data = {
            "type": atom_type,
            "name": atom_name,
            "tv": truth_value or [1.0, 1.0],  # Default truth value
            "timestamp": datetime.now().isoformat(),
        }
        
        if outgoing_set:
            atom_data["outgoing"] = outgoing_set
        
        # Store the atom
        self.storage[atom_id] = atom_data
        return atom_id
    
    def get_atom(self, atom_id):
        """Get an atom by ID"""
        return self.storage.get(atom_id)
    
    def get_atoms_by_type(self, atom_type):
        """Get all atoms of a specific type"""
        return [atom for atom_id, atom in self.storage.items() if atom["type"] == atom_type]
    
    def get_atoms_by_name(self, atom_name):
        """Get all atoms with a specific name"""
        return [atom for atom_id, atom in self.storage.items() if atom.get("name") == atom_name]
    
    def _generate_atom_id(self, atom_type, atom_name, outgoing_set=None):
        """Generate a unique ID for an atom"""
        if outgoing_set:
            return f"{atom_type}:{','.join(outgoing_set)}"
        else:
            return f"{atom_type}:{atom_name}"
    
    def find_links_containing_atom(self, atom_id):
        """Find all links that contain a specific atom"""
        return [atom for atom_id, atom in self.storage.items() 
                if "outgoing" in atom and atom_id in atom["outgoing"]]
    
    def query_pattern(self, pattern):
        """Query atoms matching a pattern
        Pattern is a dict with constraints on atom type, name, etc.
        """
        results = []
        for atom_id, atom in self.storage.items():
            match = True
            for key, value in pattern.items():
                if key not in atom or atom[key] != value:
                    match = False
                    break
            if match:
                results.append(atom)
        return results
    
    def cleanup(self):
        """Clean up temporary files"""
        try:
            self.save_to_disk()
            if self.db_path.startswith(tempfile.gettempdir()):
                shutil.rmtree(self.db_path)
                print(f"Removed temporary database at {self.db_path}")
        except Exception as e:
            print(f"Error during cleanup: {e}")

class MockAtomSpace:
    """Mock implementation of AtomSpace"""
    def __init__(self, storage=None):
        """Initialize the mock AtomSpace"""
        if storage is None:
            self.storage = MockRocksDBStore()
        else:
            self.storage = storage
        
        # Keep an in-memory cache of atoms for faster access
        self.atoms = {}
        self.links = {}
        
        # Load atoms from storage into memory
        self._sync_from_storage()
    
    def _sync_from_storage(self):
        """Sync atoms from storage into memory"""
        # In a real implementation, this would be more sophisticated
        for atom_id, atom_data in self.storage.storage.items():
            self.atoms[atom_id] = atom_data
            if "outgoing" in atom_data:
                self.links[atom_id] = atom_data
    
    def add_node(self, node_type, node_name, truth_value=None):
        """Add a node to the AtomSpace"""
        atom_id = self.storage.store_atom(node_type, node_name, truth_value)
        self.atoms[atom_id] = self.storage.get_atom(atom_id)
        return atom_id
    
    def add_link(self, link_type, outgoing_set, truth_value=None):
        """Add a link to the AtomSpace"""
        atom_id = self.storage.store_atom(link_type, None, truth_value, outgoing_set)
        self.atoms[atom_id] = self.storage.get_atom(atom_id)
        self.links[atom_id] = self.atoms[atom_id]
        return atom_id
    
    def get_node(self, node_type, node_name):
        """Get a node by type and name"""
        atom_id = self.storage._generate_atom_id(node_type, node_name)
        return self.atoms.get(atom_id)
    
    def get_link(self, link_type, outgoing_set):
        """Get a link by type and outgoing set"""
        atom_id = self.storage._generate_atom_id(link_type, None, outgoing_set)
        return self.atoms.get(atom_id)
    
    def get_outgoing(self, link_id):
        """Get outgoing set of a link"""
        link = self.atoms.get(link_id)
        if link and "outgoing" in link:
            return link["outgoing"]
        return []
    
    def get_incoming(self, atom_id):
        """Get incoming set of an atom"""
        return [link_id for link_id, link in self.links.items() 
                if "outgoing" in link and atom_id in link["outgoing"]]
    
    def query_pattern(self, pattern):
        """Execute a pattern match query"""
        # In a real implementation, this would use the PatternMatcher
        return self.storage.query_pattern(pattern)
    
    def save(self):
        """Save the AtomSpace to storage"""
        self.storage.save_to_disk()
    
    def print_atom(self, atom_id):
        """Print an atom in Scheme format"""
        atom = self.atoms.get(atom_id)
        if not atom:
            return f";; Atom {atom_id} not found"
        
        if "outgoing" in atom:
            # Link format
            tv_str = f"(stv {atom['tv'][0]:.6f} {atom['tv'][1]:.6f})"
            result = f"({atom['type']} {tv_str}\n"
            for out_id in atom["outgoing"]:
                out_atom = self.atoms.get(out_id)
                if out_atom:
                    if "name" in out_atom:
                        result += f"  ({out_atom['type']} \"{out_atom['name']}\")\n"
                    else:
                        result += f"  {self.print_atom(out_id)}\n"
            result += ")"
            return result
        else:
            # Node format
            tv_str = f"(stv {atom['tv'][0]:.6f} {atom['tv'][1]:.6f})"
            return f"({atom['type']} \"{atom['name']}\" {tv_str})"
    
    def print_all_atoms(self):
        """Print all atoms in the AtomSpace"""
        result = ""
        # First print nodes
        nodes = {atom_id: atom for atom_id, atom in self.atoms.items() if "outgoing" not in atom}
        for atom_id, atom in nodes.items():
            result += self.print_atom(atom_id) + "\n"
        
        # Then print links
        for atom_id, atom in self.links.items():
            result += self.print_atom(atom_id) + "\n"
        
        return result
    
    def cleanup(self):
        """Clean up resources"""
        self.storage.cleanup()

def create_knowledge_base(atomspace):
    """Create a knowledge base with animal taxonomy"""
    print("\nCreating knowledge base...")
    
    # Add concepts
    animal = atomspace.add_node("ConceptNode", "animal", [0.9, 1.0])
    mammal = atomspace.add_node("ConceptNode", "mammal", [0.9, 1.0])
    bird = atomspace.add_node("ConceptNode", "bird", [0.9, 1.0])
    reptile = atomspace.add_node("ConceptNode", "reptile", [0.9, 1.0])
    
    dog = atomspace.add_node("ConceptNode", "dog", [1.0, 1.0])
    cat = atomspace.add_node("ConceptNode", "cat", [1.0, 1.0])
    eagle = atomspace.add_node("ConceptNode", "eagle", [1.0, 1.0])
    snake = atomspace.add_node("ConceptNode", "snake", [1.0, 1.0])
    
    # Add inheritance relationships
    atomspace.add_link("InheritanceLink", [mammal, animal], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [bird, animal], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [reptile, animal], [1.0, 1.0])
    
    atomspace.add_link("InheritanceLink", [dog, mammal], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [cat, mammal], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [eagle, bird], [1.0, 1.0])
    atomspace.add_link("InheritanceLink", [snake, reptile], [1.0, 1.0])
    
    # Add capabilities
    can_fly = atomspace.add_node("PredicateNode", "can_fly", [1.0, 1.0])
    has_fur = atomspace.add_node("PredicateNode", "has_fur", [1.0, 1.0])
    
    # Create eval links for capabilities
    # Eagle can fly
    eagle_fly_list = atomspace.add_link("ListLink", [eagle], [1.0, 1.0])
    atomspace.add_link("EvaluationLink", [can_fly, eagle_fly_list], [1.0, 1.0])
    
    # Birds generally can fly (but with lower confidence)
    bird_fly_list = atomspace.add_link("ListLink", [bird], [1.0, 1.0])
    atomspace.add_link("EvaluationLink", [can_fly, bird_fly_list], [0.95, 0.8])
    
    # Dogs and cats have fur
    dog_fur_list = atomspace.add_link("ListLink", [dog], [1.0, 1.0])
    atomspace.add_link("EvaluationLink", [has_fur, dog_fur_list], [1.0, 1.0])
    
    cat_fur_list = atomspace.add_link("ListLink", [cat], [1.0, 1.0])
    atomspace.add_link("EvaluationLink", [has_fur, cat_fur_list], [1.0, 1.0])
    
    # Save to storage
    atomspace.save()
    print("Knowledge base created and saved to storage")

def pattern_match_demo(atomspace):
    """Demonstrate pattern matching capabilities"""
    print("\nPerforming pattern matching queries...")
    
    # Query 1: Find all mammals
    print("\nQuery: Find all mammals")
    mammal = atomspace.get_node("ConceptNode", "mammal")
    if mammal:
        mammal_id = atomspace.storage._generate_atom_id("ConceptNode", "mammal")
        # Find inheritance links where mammal is the target
        pattern = {"type": "InheritanceLink"}
        matches = atomspace.query_pattern(pattern)
        
        mammals = []
        for match in matches:
            if "outgoing" in match and len(match["outgoing"]) == 2:
                outgoing = match["outgoing"]
                # Check if second element is mammal_id (points to mammal)
                if outgoing[1] == mammal_id:
                    # Get the source node
                    source_id = outgoing[0]
                    source = atomspace.atoms.get(source_id)
                    if source:
                        mammals.append(source["name"])
        
        print(f"Found mammals: {', '.join(mammals)}")
    
    # Query 2: Find animals that can fly
    print("\nQuery: Find animals that can fly")
    can_fly = atomspace.get_node("PredicateNode", "can_fly")
    if can_fly:
        can_fly_id = atomspace.storage._generate_atom_id("PredicateNode", "can_fly")
        
        # Find evaluation links where can_fly is the predicate
        pattern = {"type": "EvaluationLink"}
        matches = atomspace.query_pattern(pattern)
        
        flyers = []
        for match in matches:
            if "outgoing" in match and len(match["outgoing"]) == 2:
                outgoing = match["outgoing"]
                # Check if first element is can_fly_id
                if outgoing[0] == can_fly_id:
                    # Second element should be a ListLink
                    list_id = outgoing[1]
                    list_link = atomspace.atoms.get(list_id)
                    if list_link and "outgoing" in list_link:
                        # Get the items in the list
                        for item_id in list_link["outgoing"]:
                            item = atomspace.atoms.get(item_id)
                            if item:
                                flyers.append(item["name"])
        
        print(f"Found flying animals: {', '.join(flyers)}")
    
    # Query 3: Find all animals (through inheritance hierarchy)
    print("\nQuery: Find all animals (through inheritance hierarchy)")
    animal = atomspace.get_node("ConceptNode", "animal")
    if animal:
        animal_id = atomspace.storage._generate_atom_id("ConceptNode", "animal")
        
        # This is a recursive query that would normally use the Pattern Matcher
        # For our mock, we'll do it manually
        
        # First, find direct inheritance links to animal
        pattern = {"type": "InheritanceLink"}
        matches = atomspace.query_pattern(pattern)
        
        # Start with animal categories
        animal_categories = []
        for match in matches:
            if "outgoing" in match and len(match["outgoing"]) == 2:
                outgoing = match["outgoing"]
                if outgoing[1] == animal_id:
                    source_id = outgoing[0]
                    source = atomspace.atoms.get(source_id)
                    if source:
                        animal_categories.append(source["name"])
        
        # Now find animals in each category
        all_animals = []
        for category in animal_categories:
            category_id = atomspace.storage._generate_atom_id("ConceptNode", category)
            
            for match in matches:
                if "outgoing" in match and len(match["outgoing"]) == 2:
                    outgoing = match["outgoing"]
                    if outgoing[1] == category_id:
                        source_id = outgoing[0]
                        source = atomspace.atoms.get(source_id)
                        if source:
                            all_animals.append(source["name"])
        
        print(f"Animal categories: {', '.join(animal_categories)}")
        print(f"Individual animals: {', '.join(all_animals)}")

def demo_persistence(db_path):
    """Demonstrate persistence with RocksDB"""
    print("\nDemonstrating AtomSpace persistence with RocksDB...")
    
    # Create a new AtomSpace with storage
    storage = MockRocksDBStore(db_path)
    atomspace = MockAtomSpace(storage)
    
    # Create knowledge base
    create_knowledge_base(atomspace)
    
    # Show the full AtomSpace
    print("\nCurrent AtomSpace contents:")
    print(atomspace.print_all_atoms())
    
    # Perform pattern matching
    pattern_match_demo(atomspace)
    
    # Save and close
    atomspace.save()
    
    # Reopen to demonstrate persistence
    print("\nReopening AtomSpace from storage...")
    new_storage = MockRocksDBStore(db_path)
    new_atomspace = MockAtomSpace(new_storage)
    
    # Show that data was persisted
    print("\nAtomSpace contents after reopening:")
    print(new_atomspace.print_all_atoms())
    
    # Clean up
    new_atomspace.cleanup()

def main():
    """Main function to run the demo"""
    print("OpenCog Basic AtomSpace with RocksDB Persistence Demo")
    print("====================================================")
    
    # Create a persistent storage location
    db_path = os.path.join(tempfile.gettempdir(), "atomspace_rocksdb_demo")
    
    # Run the persistence demo
    demo_persistence(db_path)
    
    print("\nDemo completed successfully")

if __name__ == "__main__":
    main() 