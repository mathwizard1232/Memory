#ifndef _DATABASE
#define _DATABASE

// Represents the connection to the mongodb database
// mongod must have already been run

#include <mongo/client/dbclient.h>
#include <vector>

//using namespace mongo; //bad form

using mongo::Query;
using mongo::BSONObj;
using mongo::DBClientConnection;
typedef mongo::auto_ptr<mongo::DBClientCursor> cursor;

class Database {
 public:
  Database();

  // Add a given user to the database
  void add_user(const char str[]);

  // Add the given object to the given collection
  void insert(const char coll[], BSONObj o);

  // Returns an array of strings for existing users.
  vector<const char*> users();

 private:
  // Run a given query
  cursor query(const char collection[], 
               const Query q);

  // Return the full collection
  cursor query(const char collection[]);

  // From the given cursor, return an array of
  // strings where each line represents the given
  // field of a unique result.
  vector<const char*> extract(cursor c, char field[]);

  DBClientConnection c;
};

#endif
