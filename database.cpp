#include "card.h"
#include "utility.h"
using std::string;

Database::Database() {
  c.connect("localhost.localdomain");
}

void Database::add_user(const char str[]) {
  BSONObj p = BSON("name" << str);
  c.insert("memory.users",p);
}

string Database::insert(const char coll[], BSONObj o) {
  cursor cur;
  cur = query(coll,o);
  if (!cur->more()) {
    c.insert(coll,o);
    cur = c.query(coll,o);
  } else {
    log("Attempted to reinsert element");
  }
  BSONObj post = cur->next();
  BSONElement id;
  post.getObjectID(id);
  return id.__oid().str();
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

// Allocates memory. User must ultimately free this to avoid a leak.
vector<const char*> Database::extract(cursor c, char field[]) {
  vector<const char*> results;
  while (c->more()) {
    BSONObj p = c->next();
    const char* curr = p.getStringField(field);
    char* temp = new char[length(curr)];
    copy(curr, temp);
    results.push_back(temp);
  }
  return results;
}

Card* Database::next_review(char* user) {
  Query q = QUERY("active" << true << "user" << user);
  q.sort(BSON("next_review" << 1 << "_id" << -1));
  cursor c = query("memory.data",q);
  if (c->more()) {
    BSONObj n = c->next();
    Card* r = Card::CardFactory(n,this);
    if (r == null) {
      log("Bad factory input in Database::next_review");
    }
    return r;
  } else {
    return null;
  }
}

char* readString(BSONObj b, const char* f) {
  char* temp;
  copy_leak(b.getStringField(f),temp);
  return temp;
}

int readInt(BSONObj b, const char* f) {
  return b.getIntField(f);
}

bool readBool(BSONObj b, const char* f) {
  return b.getBoolField(f);
}

void Database::directUpdate(const char* collection, Query q, BSONObj o) {
  c.update(collection,q,o);
}

// Atomicly update only given fields
void Database::update(const char* collection, string id, BSONObj change) {
  string q = "{\"_id\":ObjectId(\""+id+"\")}";
  update(collection, Query(q), change);
}

void Database::update(const char* collection, Query q, BSONObj update) {
  string u = "{$set : " + update.jsonString() + "}";
  BSONObj o = mongo::fromjson(u);
  directUpdate(collection,q,o);
}

BSONObj Database::get(const char* collection, string id) {
  string q = "{\"_id\":ObjectId(\""+id+"\")}";
  cursor c = query(collection,Query(q));
  return c->next();
}

int Database::getInt(const char* collection, string id, const char* field) {
  return readInt(get(collection,id),field);
}

std::string readId(BSONObj b) {
  BSONElement i;
  b.getObjectID(i);
  return i.__oid().str();
}

void Database::update(const char* collection, BSONElement& e, BSONObj change) {
  mongo::OID id = e.__oid();
  update(collection, id.str(), change);
}
using mongo::BSONObjBuilder;
void Database::update(const char* collection, string id, const char* field, vector<string> data) {
  BSONObjBuilder b;
  for (int i = 0; i < data.size(); i++) {
    b.appendElements(BSON(i_str(i) << data[i].c_str()));
  }
  BSONObj o = b.done();
  BSONObjBuilder c;
  c.appendArray(field, o);
  BSONObj p = c.done();
  update(collection, id, p);
}

vector<string> Database::b_arr(BSONObj arr) {
  vector<string> out;
  int index = 0;
  while (arr.hasField(i_cstr(index))) {
    out.push_back(arr.getStringField(i_cstr(index)));
    index++;
  }
  return out;
}

/*vector<BSONObj> Database::getAll(const char* user) {
  Query q = QUERY("active" << true << "user" << user);
  q.sort(BSON("_id" << 1));
  cursor c = query("memory.data",q);
  
  vector<BSONObj> out;
  while (c->more()) {
    BSONObj n = c->next();
    out.push_back(n);
  }

  return out;
}

void Database::dump(ofstream &o, const char* user) {
  vector<BSONObj> all;
  all = getAll(user);
  for (int i = 0; i < all.size(); i++) {
    try {
      Card* r = Card::CardFactory(all[i],this);
    } catch (...) {
      log("Exception caught.");
    }
    //    r->write(o);
  }
  }*/ // code above does not work mysteriously

void Database::dump(ofstream &o, const char* user) {
  vector<Card*> all = getCards(user);
  for (int i = 0; i < all.size(); i++) {
    all[i]->write(o);
  }
}

vector<Card*> Database::getCards(const char* user) {
  Query q = QUERY("active" << true << "user" << user);
  q.sort(BSON("_id" << 1));
  cursor c = query("memory.data",q);
  
  vector<Card*> out;
  while (c->more()) {
    out.push_back(Card::CardFactory(c->next(),this));
  }
  return out;
}
