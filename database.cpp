#include "database.h"

Database::Database() {
  c.connect("localhost.localdomain");
}

void Database::add_user(const char str[]) {
  BSONObj p = BSON("name" << str);
  c.insert("memory.users",p);
}

void Database::insert(const char coll[], BSONObj o) {
  c.insert(coll,o);
}

vector<const char*> Database::users() {
  return extract(query("memory.users"),
                 (char*)"name");
}

cursor Database::query(const char coll[],
                       const Query q) {
  return c.query(coll,q);
}

cursor Database::query(const char coll[]) {
  return query(coll, BSONObj());
}

vector<const char*> Database::extract(cursor c, char field[]) {
  vector<const char*> results;
  while (c->more()) {
    BSONObj p = c->next();
    results.push_back(p.getStringField(field));
  }
  return results;
}
