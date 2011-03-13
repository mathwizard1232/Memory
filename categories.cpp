#include "categories.h"
#include "database.h"
#include "utility.h"

using std::string;

Category Category::root;// = new Category();

Category::Category()
  :parent(null), name("all"), suffix(" ")
{
}

Category::~Category()
{
  int i;
  for (i = 0; i < children.size(); i++) {
    delete children[i];
  }
}

string Category::toString() const {
  return name;
}

Category* Category::makeChild(string name) {
  Category* c = new Category();
  c->name = name;
  c->parent = this;
  children.push_back(c);
  return c;
}

void Category::setSuffix(string s) {
  suffix = s;
}

Category* Category::findBelow(string target) {
  if (toString() == target) {
    return this;
  }

  Category* result;
  for (int i = 0; i < children.size(); i++) {
    result = children[i]->findBelow(target);
    if (result != null) {
      return result;
    }
  }
  return null; // Not found here or below.
}

Category* Category::find(string name) {
  if (name == "null") {
    return Category::all();
  }

  Category* ans = Category::all()->findBelow(name);
  if (ans == null) {
    log(name.c_str());
    log("could not find ^ in Category::find");
  }

  return ans;
}

void Category::add(string p, string n, string s) {
  Category* parent = find(p);
  Category* addee = parent->makeChild(n);
  addee->setSuffix(s);
}

// Note that "all" does not exist in the database. This is because there would
// be no way to recreate it as the others are recreated. So insert on "all" would
// be bad.
void Category::insert(Database* db) {
  db->insert("memory.categories",BSON("name"<<name<<"suffix"<<suffix<<"parent"<<parent->toString()));
}

void Category::update(Database* db) {
  db->update("memory.categories",QUERY("name"<<name),BSON("suffix"<<suffix));
}

Category* Category::all() {
  return &root;
}

bool Category::rootp() {
  return toString() == "all";
}

string Category::getSuffix() {
  return suffix;
}

Category* Category::getParent() {
  return parent;
}

vector<Category*> Category::getChildren() {
  return children;
}

// This should probably be rolled into database code somehow.
void Category::load(Database* d) {
  Query q;
  q.sort(BSON("_id" << 1));
  cursor c = d->query("memory.categories",q);

  // "all" already exists
  Category* p; // The parent of the next import
  while (c->more()) {
    BSONObj next = c->next();
    p = Category::find(readString(next, "parent"));
    p = p->makeChild(readString(next,"name"));
    p->setSuffix(readString(next,"suffix"));
  }
}
