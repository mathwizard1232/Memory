#ifndef _DATABASE
#define _DATABASE

// Represents the connection to the mongodb database
// mongod must have already been run

#include <mongo/client/dbclient.h>
#include <vector>
#include <string>
#include <fstream>
class Card;

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
  void directUpdate(const char* collection, Query q, BSONObj o); // wipes everything else currently in object. Use atomic updates below.
  // Atomic update rather than entire erase.
  void update(const char* collection, Query q, BSONObj o);
  void update(const char* collection, std::string id, BSONObj up);
  void update(const char* collection, BSONElement& e, BSONObj o);
  void update(const char* collection, std::string id, const char* field, vector<std::string> data);

  vector<std::string> b_arr(BSONObj arr); // Convert a BSON array to a vector
  BSONObj get(const char* collection, std::string id);
  int getInt(const char* collection, std::string id, const char* field); // Read the int value from the given field in given obj
  //  vector<BSONObj> getAll(const char* user);

  vector<Card*> getCards(const char* user);
  
  void dump(std::ofstream &o, const char* user);

  // Made public for category::load
  // return to private eventually?
  // Run a given query
  cursor query(const char collection[], 
               const Query q);

 private:

  // Return the full collection
  cursor query(const char collection[]);

  // From the given cursor, return an array of
  // strings where each line represents the given
  // field of a unique result.
  vector<const char*> extract(cursor c,const char field[]);

  DBClientConnection c;
};

// Utility functions for reading from BSONObjs
int readInt(BSONObj b, const char* field);
std::string readString(BSONObj, const char* f);
bool readBool(BSONObj, const char*);
std::string readId(BSONObj);

#endif
