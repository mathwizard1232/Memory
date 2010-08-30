#ifndef CARD_H
#define CARD_H

#include "print.h"
#include "database.h"
#include <string>
#include <vector>
#include "utility.h"

using mongo::BSONElement;

// Represents a 'flashcard' used in the Memory system.
class Card {
 public:
  Card(const char prompt[]);
  Card(BSONObj b, Database* d);
  // Card representing sequential data.
  // prompt, ans, then one or two ids to unlock after the card is memorized
  Card(const char prompt[], const char ans[], const std::string unlock1, const std::string unlock2 = "");
  
  void response(const char answer[]);
  // Make the card into a poem type from given file
  void poem(const char file[]);
  std::string decompose(std::string id,std::vector<std::string>& parts); // make constituent cards and store them as well. If id provided, use to unlock in correct order. String returned is id of first element.

  std::string insert(Database* db, char user[]);
  void print(Print* p);

  void review(Print* p);
  // Returns 0 to have the next message sent here.
  // 1 to load the next card to use
  // 2 to quit to the menu
  int review_msg(char c);
  int grade(char c);

  void updateTime(int a); // Given the grade a, figure out the next time to show the card.

  static void unlock(std::string id);
  static void front(std::string id, int offset = 0);

  static void setComponents(std::string id, std::vector<std::string> comp);

 private:
  std::string unlock1; // if sequential, these represent cards to unlock when memorized
  std::string unlock2; 
  bool active; // If true, included in reviews. If false, not. Right now, this is useful for decomposed elements where we don't review the later / full data until the first is memorized.
  char* user;
  int review_state;
  Print* p;
  char* prompt;
  char* ans;
  int type;
  char* author;
  enum {standard, // prompt & ans, basic flashcard
        poe, // prompt == title, ans == text, author set
        sequential // unlock1 & 2 are unlocked when card is "memorized". Otherwise, as standard.
  };
  int next_review;
  static Database* db;
  std::string id;
  int last_interval; // Previous number of seconds used before reviewing. Ignores whether this card was reviewed early or late.
  bool previous_success; // True if last g >= 4, false if last g <= 2. Skip history of 3s.

  std::vector<std::string> components; // Vector of the ids of subelements to this item. If the item gets a 1 or 2, move these components up.
};

#endif
