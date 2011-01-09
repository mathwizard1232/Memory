/**
 * Sequential
 * A representation of sequential knowledge, where learning x_{i} implies readiness for x_{i+1}.
 */
#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include "card.h"

class Sequential : public Card {
 public:
  Sequential();
  // prompt, ans, then one or two ids to unlock after the card is memorized
  Sequential(const char prompt[], const char ans[], const std::string unlock1, const std::string unlock2 = "");

  void finishRead(BSONObj b, Database* d);
  std::string decompose(std::string id,std::vector<std::string>& parts); // make constituent cards and store them as well. If id provided, use it as the final unlock. String returned is id of first element.
  std::string insert(Database* db, char user[], std::string& this_id);

  // Perform unlocks based on grade.
  void polyUpdate(int g);

 private:
  std::string unlock1; // These represent cards to unlock after the current one is memorized.
  std::string unlock2; 
};

#endif
