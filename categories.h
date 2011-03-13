// New class created 1/15/11
// Catgories represent the organization of the knowledge base
// This is a single inheritence multilevel organization. All cards have the default "all" category.
// All other classes descend from "all" in some way.

#ifndef CATEGORIES_H
#define CATEGORIES_H

#include <string>
#include <vector>
#include "database.h"

class Category {
 public:
  ~Category();
  // Loads the current categories from the database
  static void load(Database* d);

  // Returns the "all" category.
  static Category* all();
  bool rootp(); // Predicate determining whether this is the "all" category
  
  // All categories have a name
  std::string toString() const;

  // Create a descendant of this class with the given name.
  Category* makeChild(std::string name);
  // If the above were called because of new user input, then the following is needed to commit it.
  void insert(Database* db);

  // Set the suffix for this category
  void setSuffix(std::string s);
  // Similarly, when the above is called because of a user change, we must also inform the db.
  void update(Database* db);

  std::string getSuffix();
  Category* getParent();

  // Finds the given category. A simple DFS is used, because few categories are anticipated.
  static Category* find(std::string name);
  Category* findBelow(std::string name);

  // This is used when importing from the database. We know the parent, add the child.
  void add(std::string parent, std::string name, std::string suffix);

  std::vector<Category*> getChildren();

 private:
  Category* parent;
  std::string name;
  std::string suffix; // This is appended to all the prompts in this category.
  std::vector<Category*> children; // Used for searching.

  // Ensures that there is only one root made.
  Category();
  static Category root;
};

  // A category is uniquely identified by its name.
//bool operator == (const Category &lhs, const Category &rhs)  {
//  return lhs.toString() == rhs.toString();
//}
#endif
