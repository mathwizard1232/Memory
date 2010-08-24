#ifndef _DATABASE
#define _DATABASE

// Represents the connection to the mongodb database
// mongod must have already been run

#include <mongo/client/dbclient.h>
#include <vector>
#include <string>
class Card;
//#include "card.h"
//using namespace mongo; //bad form

using mongo::Query;
using mongo::BSONObj;
using mongo::BSONElement;
using mongo::DBClientConnection;
typedef mongo::auto_ptr<mongo::DBClientCursor> cursor;

using std::vector;

class Database {
 public:
  Database();

  // Add a given user to the database
  void add_user(const char str[]);

  // Add the given object to the given collection
  std::string insert(const char coll[], BSONObj o);

  // Returns an array of strings for existing users.
  vector<const char*> users();

  // Returns the card which is to be reviewed next for the given user.
  Card* next_review(char* user);
  void update(const char* collection, Query q, BSONObj o);
  // Atomic update rather than entire erase.
  void update(const char* collection, std::string id, BSONObj up);
  void update(const char* collection, BSONElement& e, BSONObj o);

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

// Utility functions for reading from BSONObjs
int readInt(BSONObj b, const char* field);
char* readString(BSONObj, const char* f);

#endif
