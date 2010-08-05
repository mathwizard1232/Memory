#include "card.h"
#include "utility.h"
#include <fstream>
#include <string>
#include <boost/regex.hpp>

using std::ifstream;
using std::stringstream;
using std::string;

Card::Card(const char p[])
  :type(standard)
{
  prompt = new char[length(p)];
  copy(p,prompt);
}

void Card::response(const char r[]) {
  ans = new char[length(r)];
  copy(r,ans);
}

void Card::insert(Database* db, char user[]) {
  BSONObj a = BSON("user" << user << "prompt" << prompt << "response" << ans);
  db->insert("memory.data",a);
}

void Card::print(Print* p) {
  switch (type) {
  case standard:
    p->printf("Prompt: %s",prompt);
    p->printf("Response: %s",ans);
    break;
  case poe:
    p->printf("Title: %s",prompt);
    p->printf("Author: %s",author);
    p->printf("Text: %s",ans);
    break;
  };
}

/* Poems must be plain ASCII text.
   First line: title
   Second line: "By $author"
   Third line: blank
   Rest: poem, extra newline between stanzas */
void Card::poem(const char file[]) {
  type = poe;
  ifstream f(file);
  getline(f,prompt);
  getline(f,author);
  author += 3; // Skip "By "
  getline(f,ans); // Skip blank third line
  
  // Thanks to: http://stackoverflow.com/questions/116951/using-fstream-to-read-every-character-including-spaces-and-newline
  stringstream buffer;
  buffer << f.rdbuf();
  string contents(buffer.str());
  ans = new char[buffer.str().size() + 1];
  copy(contents.c_str(),ans);
}
