#! /usr/bin/env python3
#
# stop_go.py
#
"""
Example of creating a "behavior tree" in the AtomSpace.

Behavior trees can be thought of as long sequences of nested
if-then-else statements; the branch that is taken results in
a "behavior" being performed.

Behavior trees are usually driven by dynamic data; however, to keep
the example simple, the below is static; it simply counts green and
red lights, and halts at the first red light.

The if-then is implemented via a matching clause with the pattern
matcher. When a match is seen, the matcher moves on to the next
clause.
"""

from opencog.atomspace import *
from opencog.type_constructors import *
from opencog.logger import Logger, log

# Logging will be written to opencog.log in the current directory.
log.set_level('DEBUG')
log.info("Starting the stop-go demo")

# Tell the python type constructors where to find the AtomSpace.
set_default_atomspace(AtomSpace())

# The callback counts the number of red and green lights.
# It returns a TruthValue of TRUE for green lights and FALSE for the
# red lights.  FALSE is interpreted as a mismatch (failure to satisfy)
# by the pattner matcher, and thus, the pattern matcher will backtrack
# and search for a different solution. Since the example below contains
# no variables, it will just backtrack to the start, and then report
# non-satisfiability (which is what we want, when we get a red light).
green = 0
red = 0

def stop_go(atom):

    if atom == ConceptNode("green light"):
        print("Got a green light...")
        global green
        green += 1
        return TruthValue(1,1)

    elif atom == ConceptNode("red light"):
        print("Got a red light!")
        global red
        red += 1
        return TruthValue(0,1)

    else:
        print("Oh No!! Car wreck!")
        assert(false)

    return TruthValue(0,0)

# This is the pattern that the pattern matcher attempts to ground.
# It consists of two green lights, which evaluate to true, followed
# by a red light, which halts the satisfiability search. The last
# term is thus never encountered.  Which is a good thing.
satisfaction_handle = SatisfactionLink(
    SequentialAndLink(
        EvaluationLink(
            GroundedPredicateNode("py: stop_go"),
            ListLink(
                ConceptNode("green light")
            )
        ),
        EvaluationLink(
            GroundedPredicateNode("py: stop_go"),
            ListLink(
                ConceptNode("green light")
            )
        ),
        EvaluationLink(
            GroundedPredicateNode("py: stop_go"),
            ListLink(
                ConceptNode("red light")
            )
        ),
        EvaluationLink(
            GroundedPredicateNode("py: stop_go"),
            ListLink(
                ConceptNode("traffic ticket")
            )
        )
    )
)

# Perform the actual satisfiability search.
result = satisfaction_handle.execute()

print("Satisfaction query result:", result)
print("Number of green lights:",  green)
print("Number of red lights:",  red)
