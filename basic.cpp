#include "basic.h"

Basic::Basic()
{
}

Basic::Basic(const char p[])
{
  type = standard;
  next_review = time(null);
  active = true;
  prompt = strdup(p);
  //(char*) malloc(strlen(p));
  //copy(p,prompt);
}

void Basic::response(const char r[]) {
  ans = strdup(r);
  //(char*) malloc(strlen(r));
  //copy(r,ans);
}

// This_id is the id of this [full] card inserted. The return value is the first [element] card to be activated for this block of memory.
// For a basic flashcard, these are identical.
std::string Basic::insert(Database* d, char u[], std::string& this_id) {
  Card::db = d; // Set the database connection for this Card.
  user = strdup(u);
  //  BSONObj a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active << "category" << cat->toString());
  // BSONObj a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active << "category" << "null");//cat->toString());
 //  a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active << "category" << cat->toString());
  BSONObj a;
  char* tmp = strdup(cat->toString().c_str());
  a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active << "category" << tmp);
  this_id = Card::db->insert("memory.data",a);
  return this_id;
}

void Basic::finishRead(BSONObj b, Database* d) {
  prompt = strdup(readString(b,"prompt").c_str());
  ans = strdup(readString(b,"response").c_str());
}
