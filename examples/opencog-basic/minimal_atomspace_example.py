#!/usr/bin/env python3
"""
OpenCog Basic Minimal AtomSpace Example
This script simulates AtomSpace functionality without requiring the actual OpenCog libraries
"""

class TruthValue:
    """Simplified TruthValue class simulating OpenCog's TruthValue"""
    def __init__(self, strength=1.0, confidence=1.0):
        self.strength = strength
        self.confidence = confidence
    
    def __str__(self):
        return f"(stv {self.strength} {self.confidence})"

class Atom:
    """Base Atom class simulating OpenCog's Atom"""
    def __init__(self, atom_type, name=None, tv=None):
        self.type = atom_type
        self.name = name
        self.tv = tv or TruthValue()
        self.incoming = []
        self.id = f"{self.type}:{self.name}" if name else f"{self.type}"
    
    def __str__(self):
        if self.name:
            return f"({self.type} \"{self.name}\" {self.tv})"
        return f"({self.type} {self.tv})"

class Node(Atom):
    """Node class simulating OpenCog's Node types"""
    def __init__(self, node_type, name, tv=None):
        super().__init__(node_type, name, tv)

class Link(Atom):
    """Link class simulating OpenCog's Link types"""
    def __init__(self, link_type, outgoing, tv=None):
        super().__init__(link_type, None, tv)
        self.outgoing = outgoing
        # Add this link to the incoming set of each outgoing atom
        for atom in outgoing:
            atom.incoming.append(self)
    
    def __str__(self):
        result = f"({self.type} {self.tv}\n"
        for atom in self.outgoing:
            # Indent each outgoing atom
            atom_str = str(atom).replace("\n", "\n  ")
            result += f"  {atom_str}\n"
        result += ")"
        return result

class AtomSpace:
    """Simplified AtomSpace class simulating OpenCog's AtomSpace"""
    def __init__(self):
        self.atoms = {}
    
    def add(self, atom):
        """Add atom to the AtomSpace"""
        self.atoms[atom.id] = atom
        return atom
    
    def get_atom(self, atom_type, name=None, outgoing=None):
        """Get atom by type and name or outgoing set"""
        if outgoing:
            atom_id = f"{atom_type}"
            return self.atoms.get(atom_id)
        else:
            atom_id = f"{atom_type}:{name}"
            return self.atoms.get(atom_id)
    
    def __str__(self):
        """Print the AtomSpace contents"""
        result = "AtomSpace contents:\n"
        for atom_id, atom in self.atoms.items():
            result += str(atom) + "\n"
        return result
    
    def __len__(self):
        return len(self.atoms)

# Factory functions to simulate OpenCog's type constructors
def ConceptNode(name, tv=None):
    """Create a ConceptNode"""
    return Node("ConceptNode", name, tv)

def PredicateNode(name, tv=None):
    """Create a PredicateNode"""
    return Node("PredicateNode", name, tv)

def InheritanceLink(parent, child, tv=None):
    """Create an InheritanceLink"""
    return Link("InheritanceLink", [parent, child], tv)

def EvaluationLink(predicate, args, tv=None):
    """Create an EvaluationLink"""
    return Link("EvaluationLink", [predicate, args], tv)

def ListLink(args, tv=None):
    """Create a ListLink"""
    return Link("ListLink", args, tv)

def BindLink(variables, pattern, rewrite):
    """Create a BindLink for pattern matching"""
    return Link("BindLink", [variables, pattern, rewrite])

def pattern_match(atomspace, bind_link):
    """Simulate pattern matching in OpenCog"""
    # Extract the variables, pattern and rewrite rule from the bind link
    variables = bind_link.outgoing[0]
    pattern = bind_link.outgoing[1]
    rewrite = bind_link.outgoing[2]
    
    # This is a very simplified pattern matcher
    # In a real implementation, this would be much more complex
    results = []
    
    # For demonstration purposes, we'll just check for InheritanceLinks
    # that match the pattern structure
    if pattern.type == "InheritanceLink":
        for atom_id, atom in atomspace.atoms.items():
            if atom.type == "InheritanceLink":
                # Simple matching logic - not as sophisticated as the real PatternMatcher
                results.append(atom)
    
    # Return a ListLink containing the results
    return Link("ListLink", results)

def main():
    """Main function demonstrating AtomSpace simulation"""
    print("OpenCog Basic Minimal AtomSpace Example")
    print("=======================================")
    
    # Create an AtomSpace
    atomspace = AtomSpace()
    
    # Create some nodes
    human = atomspace.add(ConceptNode("human", TruthValue(0.9, 0.8)))
    mammal = atomspace.add(ConceptNode("mammal", TruthValue(0.9, 0.9)))
    animal = atomspace.add(ConceptNode("animal", TruthValue(0.9, 0.9)))
    
    alice = atomspace.add(ConceptNode("Alice", TruthValue(1.0, 1.0)))
    bob = atomspace.add(ConceptNode("Bob", TruthValue(1.0, 1.0)))
    
    # Create some links
    atomspace.add(InheritanceLink(human, mammal, TruthValue(1.0, 0.9)))
    atomspace.add(InheritanceLink(mammal, animal, TruthValue(1.0, 0.9)))
    atomspace.add(InheritanceLink(alice, human, TruthValue(1.0, 0.9)))
    atomspace.add(InheritanceLink(bob, human, TruthValue(1.0, 0.9)))
    
    # Create a predicate
    likes = atomspace.add(PredicateNode("likes"))
    
    # Create some evaluations
    alice_bob_list = atomspace.add(ListLink([alice, bob]))
    bob_alice_list = atomspace.add(ListLink([bob, alice]))
    
    atomspace.add(EvaluationLink(likes, alice_bob_list))
    atomspace.add(EvaluationLink(likes, bob_alice_list))
    
    # Print the AtomSpace
    print("\nAtomSpace after adding nodes and links:")
    print(atomspace)
    
    # Demonstrate pattern matching
    print("\nPattern Matching Example:")
    print("Finding all inheritance relationships...")
    
    # Create a simple pattern to match all inheritance links
    var_x = atomspace.add(Node("VariableNode", "$X"))
    var_y = atomspace.add(Node("VariableNode", "$Y"))
    
    # Pattern to match: ($X inherits from $Y)
    pattern = atomspace.add(InheritanceLink(var_x, var_y))
    
    # Variables declaration
    variables = atomspace.add(Link("VariableList", [var_x, var_y]))
    
    # Create a bind link
    bind_link = atomspace.add(BindLink(variables, pattern, var_x))
    
    # Execute pattern matching
    results = pattern_match(atomspace, bind_link)
    
    # Display results
    print("\nFound inheritance relationships:")
    for atom in results.outgoing:
        print(f"  {atom}")
    
    print("\nSimulated AtomSpace demonstration completed!")

if __name__ == "__main__":
    main() 