#include "basic.h"

Basic::Basic()
{
}

Basic::Basic(const char p[])
{
  type = standard;
  next_review = time(null);
  active = true;
  prompt = new char[length(p)+1];
  copy(p,prompt);
}

void Basic::response(const char r[]) {
  ans = new char[length(r)];
  copy(r,ans);
}

// This_id is the id of this [full] card inserted. The return value is the first [element] card to be activated for this block of memory.
// For a basic flashcard, these are identical.
std::string Basic::insert(Database* d, char u[], std::string& this_id) {
  Card::db = d; // Set the database connection for this Card.
  copy_leak(u,user);
  BSONObj a;
  a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active);
  this_id = Card::db->insert("memory.data",a);
  return this_id;
}

void Basic::finishRead(BSONObj b, Database* d) {
  prompt = readString(b,"prompt");
  ans = readString(b,"response");
}