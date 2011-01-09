/**
 * Basic
 * Represents the simplest form of Card: a simple prompt/ans flashcard with no inherent relationships to other cards or other metadata.
 */
#ifndef BASIC_H
#define BASIC_H

#include "card.h"

class Basic : public Card {
 public:
  Basic();
  Basic(const char prompt[]);  
  void response(const char answer[]);

  void finishRead(BSONObj b, Database* d);
  std::string insert(Database* db, char user[], std::string& this_id);
};
#endif
