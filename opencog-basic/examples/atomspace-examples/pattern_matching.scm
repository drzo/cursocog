;; =============================================================================
;; Pattern Matching with AtomSpace
;; =============================================================================
;; This script demonstrates the pattern matching capabilities of AtomSpace,
;; which is a powerful feature for knowledge mining and reasoning.
;;
;; To run this script: guile -l pattern_matching.scm
;;
;; NOTE: Make sure the required modules are installed and opencog-basic is built.

;; Load the necessary modules
(use-modules (opencog))
(use-modules (opencog exec))
(use-modules (opencog query))

;; =============================================================================
;; SECTION 1: Creating Knowledge Base
;; =============================================================================

;; Let's create a knowledge base about people, relationships, and properties
;; This will be our dataset for pattern matching

;; ---------------------------
;; People
;; ---------------------------
(Concept "Alice")
(Concept "Bob")
(Concept "Charlie")
(Concept "Dave")
(Concept "Eve")

;; ---------------------------
;; Relationships
;; ---------------------------
;; FriendOf relationships
(Evaluation
  (Predicate "FriendOf")
  (List (Concept "Alice") (Concept "Bob"))
)

(Evaluation
  (Predicate "FriendOf")
  (List (Concept "Bob") (Concept "Alice"))  ; Friendship is symmetric
)

(Evaluation
  (Predicate "FriendOf")
  (List (Concept "Alice") (Concept "Charlie"))
)

(Evaluation
  (Predicate "FriendOf")
  (List (Concept "Charlie") (Concept "Dave"))
)

(Evaluation
  (Predicate "FriendOf")
  (List (Concept "Eve") (Concept "Alice"))
)

;; ---------------------------
;; Locations
;; ---------------------------
(Concept "New York")
(Concept "San Francisco")
(Concept "London")
(Concept "Tokyo")

;; Define where people live
(Evaluation
  (Predicate "LivesIn")
  (List (Concept "Alice") (Concept "New York"))
)

(Evaluation
  (Predicate "LivesIn")
  (List (Concept "Bob") (Concept "New York"))
)

(Evaluation
  (Predicate "LivesIn")
  (List (Concept "Charlie") (Concept "San Francisco"))
)

(Evaluation
  (Predicate "LivesIn")
  (List (Concept "Dave") (Concept "London"))
)

(Evaluation
  (Predicate "LivesIn")
  (List (Concept "Eve") (Concept "Tokyo"))
)

;; ---------------------------
;; Properties
;; ---------------------------
;; Age
(Evaluation
  (Predicate "Age")
  (List (Concept "Alice") (Number 30))
)

(Evaluation
  (Predicate "Age")
  (List (Concept "Bob") (Number 35))
)

(Evaluation
  (Predicate "Age")
  (List (Concept "Charlie") (Number 25))
)

(Evaluation
  (Predicate "Age")
  (List (Concept "Dave") (Number 40))
)

(Evaluation
  (Predicate "Age")
  (List (Concept "Eve") (Number 28))
)

;; Profession
(Evaluation
  (Predicate "Profession")
  (List (Concept "Alice") (Concept "Engineer"))
)

(Evaluation
  (Predicate "Profession")
  (List (Concept "Bob") (Concept "Doctor"))
)

(Evaluation
  (Predicate "Profession")
  (List (Concept "Charlie") (Concept "Engineer"))
)

(Evaluation
  (Predicate "Profession")
  (List (Concept "Dave") (Concept "Teacher"))
)

(Evaluation
  (Predicate "Profession")
  (List (Concept "Eve") (Concept "Artist"))
)

;; =============================================================================
;; SECTION 2: Basic Pattern Matching
;; =============================================================================

;; ---------------------------
;; Example 1: Find all friends of Alice
;; ---------------------------
(define alice-friends
  (Get
    (Evaluation
      (Predicate "FriendOf")
      (List (Concept "Alice") (Variable "$person"))
    )
  )
)

(display "Friends of Alice:\n")
(display (cog-execute! alice-friends))
(newline)

;; ---------------------------
;; Example 2: Find all people who live in New York
;; ---------------------------
(define new-yorkers
  (Get
    (Evaluation
      (Predicate "LivesIn")
      (List (Variable "$person") (Concept "New York"))
    )
  )
)

(display "People living in New York:\n")
(display (cog-execute! new-yorkers))
(newline)

;; ---------------------------
;; Example 3: Find all engineers
;; ---------------------------
(define engineers
  (Get
    (Evaluation
      (Predicate "Profession")
      (List (Variable "$person") (Concept "Engineer"))
    )
  )
)

(display "Engineers:\n")
(display (cog-execute! engineers))
(newline)

;; =============================================================================
;; SECTION 3: Advanced Pattern Matching with Conjunctions
;; =============================================================================

;; ---------------------------
;; Example 4: Find all engineers who live in San Francisco
;; ---------------------------
(define sf-engineers
  (Get
    (And
      (Evaluation
        (Predicate "Profession")
        (List (Variable "$person") (Concept "Engineer"))
      )
      (Evaluation
        (Predicate "LivesIn")
        (List (Variable "$person") (Concept "San Francisco"))
      )
    )
  )
)

(display "Engineers in San Francisco:\n")
(display (cog-execute! sf-engineers))
(newline)

;; ---------------------------
;; Example 5: Find all friends of Alice who are engineers
;; ---------------------------
(define alice-engineer-friends
  (Get
    (And
      (Evaluation
        (Predicate "FriendOf")
        (List (Concept "Alice") (Variable "$person"))
      )
      (Evaluation
        (Predicate "Profession")
        (List (Variable "$person") (Concept "Engineer"))
      )
    )
  )
)

(display "Alice's friends who are engineers:\n")
(display (cog-execute! alice-engineer-friends))
(newline)

;; =============================================================================
;; SECTION 4: Pattern Matching with Variables in Multiple Positions
;; =============================================================================

;; ---------------------------
;; Example 6: Find all pairs of friends
;; ---------------------------
(define friend-pairs
  (Get
    (Evaluation
      (Predicate "FriendOf")
      (List (Variable "$person1") (Variable "$person2"))
    )
  )
)

(display "All friendship pairs:\n")
(display (cog-execute! friend-pairs))
(newline)

;; ---------------------------
;; Example 7: Find all pairs of friends who live in the same city
;; ---------------------------
(define friends-same-city
  (Get
    (And
      (Evaluation
        (Predicate "FriendOf")
        (List (Variable "$person1") (Variable "$person2"))
      )
      (Evaluation
        (Predicate "LivesIn")
        (List (Variable "$person1") (Variable "$city"))
      )
      (Evaluation
        (Predicate "LivesIn")
        (List (Variable "$person2") (Variable "$city"))
      )
    )
  )
)

(display "Friends who live in the same city:\n")
(display (cog-execute! friends-same-city))
(newline)

;; =============================================================================
;; SECTION 5: Pattern Matching with Numerical Constraints
;; =============================================================================

;; ---------------------------
;; Example 8: Find all people who are older than 30
;; ---------------------------
(define older-than-30
  (Get
    (And
      (Evaluation
        (Predicate "Age")
        (List (Variable "$person") (Variable "$age"))
      )
      (GreaterThan
        (Variable "$age")
        (Number 30)
      )
    )
  )
)

(display "People older than 30:\n")
(display (cog-execute! older-than-30))
(newline)

;; ---------------------------
;; Example 9: Find friends with age difference less than 10 years
;; ---------------------------
(define friends-similar-age
  (Get
    (And
      (Evaluation
        (Predicate "FriendOf")
        (List (Variable "$person1") (Variable "$person2"))
      )
      (Evaluation
        (Predicate "Age")
        (List (Variable "$person1") (Variable "$age1"))
      )
      (Evaluation
        (Predicate "Age")
        (List (Variable "$person2") (Variable "$age2"))
      )
      (Less
        (Abs
          (Minus
            (Variable "$age1")
            (Variable "$age2")
          )
        )
        (Number 10)
      )
    )
  )
)

(display "Friends with age difference less than 10 years:\n")
(display (cog-execute! friends-similar-age))
(newline)

;; =============================================================================
;; SECTION 6: Pattern Matching with Absence (Optional)
;; =============================================================================

;; ---------------------------
;; Example 10: Find people who are not engineers
;; ---------------------------
(define non-engineers
  (Get
    (And
      ;; Get all people
      (Evaluation
        (Predicate "Profession")
        (List (Variable "$person") (Variable "$profession"))
      )
      ;; Make sure their profession is not Engineer
      (Not
        (Equal
          (Variable "$profession")
          (Concept "Engineer")
        )
      )
    )
  )
)

(display "People who are not engineers:\n")
(display (cog-execute! non-engineers))
(newline)

;; ---------------------------
;; Example 11: Find people who have no friends (more complex)
;; ---------------------------
(define people-without-friends
  (Get
    (And
      ;; Make sure they have an age (just to ensure they're people)
      (Evaluation
        (Predicate "Age")
        (List (Variable "$person") (Variable "$age"))
      )
      ;; Make sure they don't have any friends
      (Absent
        (Evaluation
          (Predicate "FriendOf")
          (List (Variable "$person") (Variable "$anyone"))
        )
      )
      (Absent
        (Evaluation
          (Predicate "FriendOf")
          (List (Variable "$anyone") (Variable "$person"))
        )
      )
    )
  )
)

(display "People with no friends:\n")
(display (cog-execute! people-without-friends))
(newline)

;; =============================================================================
;; SECTION 7: Creating Your Own Patterns
;; =============================================================================

;; ---------------------------
;; EXERCISE 1: Find all friends of friends of Alice (social distance = 2)
;; ---------------------------
;; Hint: You need:
;; 1. Alice is friends with $friend
;; 2. $friend is friends with $friend_of_friend
;; 3. $friend_of_friend is not Alice (to avoid circular references)

;; Define your pattern here
(define friends-of-friends
  (Get
    (And
      ;; Alice is friends with $friend
      (Evaluation
        (Predicate "FriendOf")
        (List (Concept "Alice") (Variable "$friend"))
      )
      ;; $friend is friends with $friend_of_friend
      (Evaluation
        (Predicate "FriendOf")
        (List (Variable "$friend") (Variable "$friend_of_friend"))
      )
      ;; $friend_of_friend is not Alice (avoid circular references)
      (Not
        (Equal
          (Variable "$friend_of_friend")
          (Concept "Alice")
        )
      )
    )
  )
)

;; ---------------------------
;; EXERCISE 2: Find all people who share a profession but live in different cities
;; ---------------------------
;; Hint: You need:
;; 1. $person1 has profession $profession
;; 2. $person2 has the same profession
;; 3. $person1 lives in city $city1
;; 4. $person2 lives in city $city2
;; 5. $city1 is not equal to $city2
;; 6. $person1 is not the same as $person2 (to avoid self-comparisons)

;; Define your pattern here
(define same-profession-different-cities
  (Get
    (And
      ;; Both have same profession
      (Evaluation
        (Predicate "Profession")
        (List (Variable "$person1") (Variable "$profession"))
      )
      (Evaluation
        (Predicate "Profession")
        (List (Variable "$person2") (Variable "$profession"))
      )
      ;; They live in different cities
      (Evaluation
        (Predicate "LivesIn")
        (List (Variable "$person1") (Variable "$city1"))
      )
      (Evaluation
        (Predicate "LivesIn")
        (List (Variable "$person2") (Variable "$city2"))
      )
      (Not
        (Equal
          (Variable "$city1")
          (Variable "$city2")
        )
      )
      ;; Avoid comparing a person to themselves
      (Not
        (Equal
          (Variable "$person1")
          (Variable "$person2")
        )
      )
    )
  )
)

;; =============================================================================
;; USAGE EXAMPLES
;; =============================================================================
;;
;; To execute the patterns defined in this file, run:
;;
;; 1. Friends of Alice:
;;    (cog-execute! alice-friends)
;;
;; 2. People living in New York:
;;    (cog-execute! new-yorkers)
;;
;; 3. Engineers:
;;    (cog-execute! engineers)
;;
;; 4. Engineers in San Francisco:
;;    (cog-execute! sf-engineers)
;;
;; 5. Friends of Alice who are engineers:
;;    (cog-execute! alice-engineer-friends)
;;
;; 6. All friendship pairs:
;;    (cog-execute! friend-pairs)
;;
;; 7. Friends who live in the same city:
;;    (cog-execute! friends-same-city)
;;
;; 8. People older than 30:
;;    (cog-execute! older-than-30)
;;
;; 9. Friends with age difference less than 10 years:
;;    (cog-execute! friends-similar-age)
;;
;; 10. People who are not engineers:
;;     (cog-execute! non-engineers)
;;
;; 11. People with no friends:
;;     (cog-execute! people-without-friends)
;;
;; 12. Friends of friends of Alice:
;;     (cog-execute! friends-of-friends)
;;
;; 13. People with same profession but different cities:
;;     (cog-execute! same-profession-different-cities) 