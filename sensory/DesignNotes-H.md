Vector Similarity
-----------------
The previous file [DesignNotes-G](DesignNotes-G.md) pondered a general
problem of type conversion, needed to deal with mildly incompatible
streams. It uses `EqualLink` as the base example.

A different variant on this problem is that of vector similarity.
Consider a search pattern that isolates a collection of vectors,
and one wishes to compare two vectors, using any one of the conventional
comparison measures: Hamming Distance, Jacquard similarity, Cosine
Distance, Mutual Information, Conditional Probability, etc. Given
two vectors, these each produce a single floating point number, or
possibly more than one.

The goal is to write an agent that can perform this compare. It's an
"agent" because it self-describes it's own input types, and its output
types as well: the format descriptions are not external to the agent,
but are intrinsic to it. That is, the agent presents a list of
connnectors that are joinable.

### Data sources
This agent needs to get input from somewhere. But where?  Earlier
examples included IRC, xterms and the file system. Now consider another
canonical example: the query pattern. Two questions arise:
 * How to describe the data generated by the query?
 * How to turn the query into an agent?

The filesys.scm demo says "use Lookat":
```
(cog-execute! (Lookat (Type 'FileSysStream))
(cog-execute! (Lookat (Type 'FooBarStream))
```
This works great, if there is an actual Atomese `FooBarStream` type
implemented as a C++ object, having a `OutputStream::describe()` method
on it.

What if the device is itself written in Atomese (such as a Query) or
what if the device is some half-assembled network? What do we do then?

The obvious answer for the Query is to place the description at a
well-known location on the query iself. Some key, perhaps at
`(Predicate "description")`

Next: what is that description? It would be somethng like this:
```
(Choice
	(Section
		(Item "this is the left marginal")
		(ConnectorSeq
			(Connector
				(Sex "command")
				(Type 'WriteLink))
			(Connector
				(Sex "command")
				(Item "left-basis"))
			(Connector
				(Sex "reply")
				(LinkSignature
					(Type 'UnisetValue)
					(Type 'ItemNode)))))
	... ditto for right.... )
```

This needs to be connected to the Hamming distance agent. What is the
description of the Hamming distance agent that is compatible with the
above?

To open, earlier examples said "use Open":
```
   (Open (Type 'TerminalStream))
   (Open (Type 'IRChatStream)
      (SensoryNode "irc://botty@irc.libera.chat:6667"))
   (Open (Type 'TextFileStream)
      (SensoryNode "file:///tmp/demo.txt"))

```

The idea above is generalized and written down in a new github repo,
called [evidence](https://github.com/opencog/evidence). It considers not
just some generalized vector, but a hierarchicaly assembled similarity
measure, based on a collection of evidence, where each item of evidence
is again composed of smaller pieces of evidence, and so on.

The core problem is I don't know how to write down an Atomese
description of such a hierarchical collection.
