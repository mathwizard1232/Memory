#ifndef CARD_H
#define CARD_H

#include "print.h"
#include "database.h"


// Represents a 'flashcard' used in the Memory system.
class Card {
 public:
  Card(const char prompt[]);
  void response(const char answer[]);
  // Make the card into a poem type from given file
  void poem(const char file[]);

  void insert(Database* db, char user[]);
  void print(Print* p);
 private:
  char* prompt;
  char* ans;
  int type;
  char* author;
  enum {standard, // prompt & ans, basic flashcard
        poe // prompt == title, ans == text, author set
  };
};

#endif
