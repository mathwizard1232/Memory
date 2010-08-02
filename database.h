#ifndef _DATABASE
#define _DATABASE

// Represents the connection to the mongodb database
// mongod must have already been run

#include <mongo/client/dbclient.h>

using namespace mongo;

class Database {
 public:
  Database();

  // Add a given user to the database
  void add_user(const char str[]);

 private:
  DBClientConnection c;
};

#endif
