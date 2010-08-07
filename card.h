#ifndef CARD_H
#define CARD_H

#include "print.h"
#include "database.h"

using mongo::BSONElement;

// Represents a 'flashcard' used in the Memory system.
class Card {
 public:
  Card(const char prompt[]);
  Card(BSONObj b, Database* d);
  
  void response(const char answer[]);
  // Make the card into a poem type from given file
  void poem(const char file[]);

  void insert(Database* db, char user[]);
  void print(Print* p);

  void review(Print* p);
  // Returns 0 to have the next message sent here.
  // 1 to load the next card to use
  // 2 to quit to the menu
  int review_msg(char c);
  int grade(char c);

  void updateTime(int a); // Given the grade a, figure out the next time to show the card.
 private:
  int review_state;
  Print* p;
  char* prompt;
  char* ans;
  int type;
  char* author;
  enum {standard, // prompt & ans, basic flashcard
        poe // prompt == title, ans == text, author set
  };
  int next_review;
  Database* db;
  BSONElement id;
};

#endif
