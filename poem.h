#ifndef POEM_H
#define POEM_H

#include "card.h"

class Poem : public Card {
 public:
  Poem();
  // Make the card into a poem type from given file
  Poem(const char file[]);

  void finishRead(BSONObj b, Database* d);
  std::string decompose(std::string id,std::vector<std::string>& parts); // make constituent cards and store them as well. If id provided, use to unlock in correct order. String returned is id of first element.
  std::string insert(Database* db, char user[], std::string& this_id);
};

#endif
