;; =============================================================================
;; Basic AtomSpace Examples
;; =============================================================================
;; This file demonstrates fundamental AtomSpace concepts and operations for
;; beginners. Examples are well-commented to explain what's happening.
;;
;; To load this in Guile: (load "basic_atomspace.scm")
;; In CogServer: loadfile basic_atomspace.scm

;; First, let's include the basic modules needed for working with AtomSpace
(use-modules (opencog))
(use-modules (opencog exec))
(use-modules (opencog query))

;; =============================================================================
;; SECTION 1: Creating Basic Atoms
;; =============================================================================

;; Concepts are the most basic Atom type - they represent nodes in the graph
(Concept "Apple")
(Concept "Fruit")
(Concept "Red")

;; Numbers are also nodes in the AtomSpace
(Number 42)
(NumberNode 3.14159)

;; We can create relationships using Links
;; This creates a relationship "Apple is a kind of Fruit"
(Inheritance (Concept "Apple") (Concept "Fruit"))

;; We can create a relationship about a property
;; "Apple has the property of being Red"
(Evaluation 
  (Predicate "has-property") 
  (List (Concept "Apple") (Concept "Red"))
)

;; =============================================================================
;; SECTION 2: Working with TruthValues
;; =============================================================================

;; Atoms can have TruthValues which represent their confidence and strength
;; Format: (stv strength confidence)
(Concept "Banana" (stv 1.0 1.0))  ; Certain that bananas exist

;; A less certain relationship
(Inheritance 
  (Concept "Banana") 
  (Concept "Fruit") 
  (stv 0.95 0.9)  ; 95% strength, 90% confidence
)

;; Different types of TruthValues
(define apple (Concept "Apple" (stv 0.8 0.9)))  ; Simple Truth Value
;; (Concept "Example" (ctv 0.8 0.2 10))  ; Count Truth Value with count=10

;; =============================================================================
;; SECTION 3: Querying the AtomSpace
;; =============================================================================

;; Let's define more knowledge for our queries
(Inheritance (Concept "Orange") (Concept "Fruit"))
(Inheritance (Concept "Carrot") (Concept "Vegetable"))
(Inheritance (Concept "Broccoli") (Concept "Vegetable"))

(Evaluation 
  (Predicate "has-property") 
  (List (Concept "Orange") (Concept "Orange"))  ; Orange has color orange
)

(Evaluation 
  (Predicate "has-property") 
  (List (Concept "Carrot") (Concept "Orange"))  ; Carrot has color orange
)

;; A simple query - find all Fruits
(define find-fruits
  (Get
    (Inheritance
      (Variable "$x")
      (Concept "Fruit")
    )
  )
)

;; To execute this query: (cog-execute! find-fruits)

;; A more complex query - find things that are orange in color
(define find-orange-things
  (Get
    (Evaluation
      (Predicate "has-property")
      (List
        (Variable "$what")
        (Concept "Orange")
      )
    )
  )
)

;; =============================================================================
;; SECTION 4: Basic Pattern Matching
;; =============================================================================

;; Let's create a pattern to find all items and their colors
(define color-pattern
  (Get
    (Evaluation
      (Predicate "has-property")
      (List
        (Variable "$object")
        (Variable "$color")
      )
    )
  )
)

;; This next pattern introduces constraints - find only fruits with colors
(define fruit-color-pattern
  (Get
    (And
      (Evaluation
        (Predicate "has-property")
        (List
          (Variable "$fruit")
          (Variable "$color")
        )
      )
      (Inheritance
        (Variable "$fruit")
        (Concept "Fruit")
      )
    )
  )
)

;; =============================================================================
;; SECTION 5: Creating and Using Executable Links
;; =============================================================================

;; An executable link to compute the sum of two numbers
(define plus-formula
  (Lambda
    (VariableList (Variable "$X") (Variable "$Y"))
    (Plus (Variable "$X") (Variable "$Y"))
  )
)

;; Execute with:
;; (cog-execute! (Put plus-formula (List (Number 2) (Number 3))))

;; An executable link that creates a new relationship
(define make-fruit
  (Lambda
    (Variable "$X")
    (Inheritance (Variable "$X") (Concept "Fruit"))
  )
)

;; Execute with:
;; (cog-execute! (Put make-fruit (Concept "Strawberry")))

;; =============================================================================
;; SECTION 6: Practical Example - A Simple Knowledge Base
;; =============================================================================

;; Let's create a more extensive knowledge base about food

;; Categories 
(Concept "Food")
(Inheritance (Concept "Fruit") (Concept "Food"))
(Inheritance (Concept "Vegetable") (Concept "Food"))
(Inheritance (Concept "Protein") (Concept "Food"))

;; More specific items
(Inheritance (Concept "Chicken") (Concept "Protein"))
(Inheritance (Concept "Beef") (Concept "Protein"))
(Inheritance (Concept "Fish") (Concept "Protein"))

;; Properties
(Predicate "calorie-content")
(Predicate "vitamin-content")

;; Assign properties
(Evaluation 
  (Predicate "calorie-content") 
  (List (Concept "Apple") (Number 52))
)

(Evaluation 
  (Predicate "calorie-content") 
  (List (Concept "Banana") (Number 89))
)

(Evaluation 
  (Predicate "calorie-content") 
  (List (Concept "Chicken") (Number 165))
)

;; Vitamin content as examples (very simplified)
(Evaluation 
  (Predicate "vitamin-content") 
  (List (Concept "Apple") (Concept "Vitamin C"))
)

(Evaluation 
  (Predicate "vitamin-content") 
  (List (Concept "Banana") (Concept "Vitamin B6"))
)

;; A query to find low-calorie foods (less than 100 calories)
(define low-calorie-foods
  (Get
    (And
      (Evaluation
        (Predicate "calorie-content")
        (List
          (Variable "$food")
          (Variable "$calories")
        )
      )
      ;; Add a constraint on the value
      (Less
        (Variable "$calories")
        (Number 100)
      )
    )
  )
)

;; ---------------------------
;; EXERCISE FOR THE READER:
;; Try to create a query that finds fruits high in Vitamin C
;; ---------------------------

;; =============================================================================
;; Usage Examples:
;; =============================================================================
;; To execute queries:
;;
;; 1. Find all fruits:
;;    (cog-execute! find-fruits)
;;
;; 2. Find things that are orange:
;;    (cog-execute! find-orange-things)
;;
;; 3. Get all object-color pairs:
;;    (cog-execute! color-pattern)
;;
;; 4. Find low-calorie foods:
;;    (cog-execute! low-calorie-foods) 