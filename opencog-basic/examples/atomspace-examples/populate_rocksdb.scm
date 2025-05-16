;; =============================================================================
;; Populate RocksDB Example
;; =============================================================================
;; This script demonstrates how to store AtomSpace data in a RocksDB backend.
;; RocksDB is a lightweight database that provides persistence for AtomSpace.
;;
;; To run this script: guile -l populate_rocksdb.scm
;;
;; NOTE: Make sure the required modules are installed and opencog-basic is built.

;; Load basic modules
(use-modules (opencog))
(use-modules (opencog persist))
(use-modules (opencog persist-rocks))

;; =============================================================================
;; SECTION 1: Setting up the RocksDB Connection
;; =============================================================================

;; First, we need to create a RocksStorageNode
;; This specifies where the database will be stored
;; Format: (RocksStorageNode "rocks:///path/to/storage")
(define rocks-storage (RocksStorageNode "rocks:///tmp/atomspace-examples"))

;; You can check if the storage node is properly created
(display "Storage node created: ")
(display rocks-storage)
(newline)

;; Open a connection to the database
(cog-open rocks-storage)
(display "Connection opened to RocksDB\n")

;; =============================================================================
;; SECTION 2: Creating and Saving AtomSpace Data
;; =============================================================================

;; Let's create some example data similar to basic_atomspace.scm
;; but this time we'll save it to the database

;; Create basic concepts
(Concept "Grape")
(Concept "Fruit")
(Concept "Purple")
(Concept "Green")

;; Define relationships
(Inheritance (Concept "Grape") (Concept "Fruit"))

(Evaluation 
  (Predicate "has-property") 
  (List (Concept "Grape") (Concept "Purple"))
)

(Evaluation 
  (Predicate "has-property") 
  (List (Concept "Grape") (Concept "Green"))
)

;; Add nutritional information
(Evaluation 
  (Predicate "calorie-content") 
  (List (Concept "Grape") (Number 67))
)

(Evaluation 
  (Predicate "vitamin-content") 
  (List (Concept "Grape") (Concept "Vitamin K"))
)

;; Add more example data
(Concept "Watermelon")
(Inheritance (Concept "Watermelon") (Concept "Fruit"))

(Evaluation 
  (Predicate "has-property") 
  (List (Concept "Watermelon") (Concept "Green"))
)

(Evaluation 
  (Predicate "has-property") 
  (List (Concept "Watermelon") (Concept "Red"))
)

(Evaluation 
  (Predicate "calorie-content") 
  (List (Concept "Watermelon") (Number 30))
)

;; =============================================================================
;; SECTION 3: Storing Data in RocksDB
;; =============================================================================

;; Now let's save all the atoms in the AtomSpace to the database
(display "Storing atoms in RocksDB...\n")
(cog-rocks-open rocks-storage)
(store-atomspace)

;; Output status
(display "Data has been stored in RocksDB\n")

;; =============================================================================
;; SECTION 4: Verifying that Data was Stored
;; =============================================================================

;; Let's clear the AtomSpace to prove the data is stored
(display "Clearing the AtomSpace...\n")
(clear)

;; Verify the AtomSpace is empty
(display "Number of atoms in AtomSpace after clearing: ")
(display (count-all))
(newline)

;; Now let's load the data back from RocksDB
(display "Loading data from RocksDB...\n")
(load-atomspace)

;; Verify that we got our data back
(display "Number of atoms in AtomSpace after loading: ")
(display (count-all))
(newline)

;; =============================================================================
;; SECTION 5: Querying the Stored Data
;; =============================================================================

;; Let's create a query to find all fruits
(define find-fruits
  (Get
    (Inheritance
      (Variable "$x")
      (Concept "Fruit")
    )
  )
)

;; Execute the query
(display "Fruits found in the database:\n")
(display (cog-execute! find-fruits))
(newline)

;; Find items that are green
(define find-green-things
  (Get
    (Evaluation
      (Predicate "has-property")
      (List
        (Variable "$what")
        (Concept "Green")
      )
    )
  )
)

;; Execute the query
(display "Green things found in the database:\n")
(display (cog-execute! find-green-things))
(newline)

;; =============================================================================
;; SECTION 6: Cleanup
;; =============================================================================

;; Close the connection
(display "Closing database connection...\n")
(cog-close rocks-storage)

;; =============================================================================
;; Usage Instructions
;; =============================================================================
;;
;; To use RocksDB with your own AtomSpace applications:
;;
;; 1. Create a RocksStorageNode with a URI pointing to your storage location
;;    (RocksStorageNode "rocks:///path/to/storage")
;;
;; 2. Open the connection: 
;;    (cog-open storage-node)
;;
;; 3. To store all atoms:
;;    (store-atomspace)
;;
;; 4. To load all atoms:
;;    (load-atomspace)
;;
;; 5. To store specific atoms:
;;    (store-atom some-atom storage-node)
;;
;; 6. To close the connection:
;;    (cog-close storage-node)
;;
;; You can also use the cog-rocks-open and cog-rocks-close convenience functions.
;; 