/**
 * Card
 *
 * Revision started 10/15 from original alpha version.
 * Continued 12/31/10
 * First draft of revision finished for card.h on 1/1/11. Now to finish drafting other files and then begin reintegration.
 *
 * This class represents a generic flashcard in the system. It has polymorphic subclasses which implement type specific knowledge.
 */
#ifndef CARD_H
#define CARD_H

#include "print.h" // Output abstraction
#include "database.h" // Database weak abstraction, mongo/bson should be excised here eventually
#include "categories.h" // Representation for the knowledge hierarchy.
#include <string>
#include <vector>
#include "utility.h"

#include <iostream>
#include <fstream>

using mongo::BSONElement;

class Card {
 public:  
  // Creates a null (invalid) card.
  Card();
  ~Card();

  static Card* CardFactory(BSONObj b, Database* d); // Load object from database.
  virtual void finishRead(BSONObj b, Database* d); // Subclass specific database reading

  // Store this card into the database as an element of user's knowledge. The return value is the first (sub)card's id, while
  // the this_id is the id of this exact card.
  virtual std::string insert(Database* db, char user[], std::string& this_id);
  std::string insert(Database* db, char user[]); // Simple alias to the above

  // Displays this card.
  // This should be made virtual.
  void print(Print* p);

  void setCategory(Category* c);
  
  // Complete file dump. Next, write the read-in system.
  virtual void write(std::ofstream &);

  // Starts the review cycle.
  void review(Print* p);
  // Returns 0 to have the next message sent here.
  // 1 to load the next card to use
  // 2 to quit to the menu
  int review_msg(char c);
  int grade(char c);

  // Given the grade a, update the next time to show the card.
  void updateTime(int a);
  virtual void polyUpdate(int a); // Function call to allow us polymorphic time updates.

  static void unlock(std::string id); // Make the card active, i.e., include it in repetitions.
  static void front(std::string id, int offset = 0); // Move the given card to the current time + offset.
  static void setDB(Database* d); // Set the database handle.
  static void setComponents(std::string id, std::vector<std::string> comp); // Update the database with id made of comp.

  // Subclasses can define how their knowledge is broken down.
  // Id is the previous id. Parts are the components to be decomposed. The return value is the id of the first element.
  virtual std::string decompose(std::string id,std::vector<std::string>& parts);

  // Remove the current card from the database.
  void delete();
 protected:
  static Database* db;
  static Print* p;

  std::string id;
  Category* cat; // How this piece of knowledge is categorized.

  char* prompt;
  char* ans;  
  bool active; // If true, included in reviews. If false, not.

  char* user; // This is the name of the current user who owns this card.
  
  // For backwards compatibility with the database, new enumerated types must be added at the end.
  typedef enum x{standard, // prompt & ans, basic flashcard // Perhaps this will stay? Others subclasses
        poe, // prompt == title, ans == text, author set
        sequential, // unlock1 & 2 are unlocked when card is "memorized". Otherwise, as standard.
        nil // This is the type of the base Card class. It is not an actual card.
  } CardType;
  CardType type;

  int next_review; // Scheduled next time to review this knowledge.
  int last_interval; // Previous number of seconds used before reviewing. Ignores whether this card was reviewed early or late.
  bool previous_success; // True if last g >= 4, false if last g <= 2. Skip history of 3s.
  int review_state; // Keeps track of the current state during review

  // Represents the original author of the knowledge, for example, in poems.
  // This should be moved to Card, but waitig on refactoring Print.
  char* author; // Eventually, this should be part of an expanded metadata process.

  std::vector<std::string> components; // Vector of the ids of subelements to this item.
};

#endif
