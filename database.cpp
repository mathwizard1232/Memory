#include "database.h"

Database::Database() {
  c.connect("localhost.localdomain");
}

void Database::add_user(const char str[]) {
  BSONObj p = BSON("name" << str);
  c.insert("memory.users",p);
}
