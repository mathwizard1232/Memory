#include "card.h"
#include "utility.h"
#include <fstream>
#include <string>
#include <boost/regex.hpp>
#include "time.h"

using std::ifstream;
using std::stringstream;
using std::string;

Card::Card(const char p[])
  :type(standard), next_review(time(null))
{
  prompt = new char[length(p)];
  copy(p,prompt);
  log("Created");
}

Card::Card(BSONObj b, Database* d) {
  db = d;
  b.getObjectID(id);
  review_state = 0;
  type = readInt(b,"type");
  next_review = readInt(b,"next_review");
  switch (type) {
  case standard:
    prompt = readString(b,"prompt");
    ans = readString(b,"response");
    break;
  case poe:
    prompt = readString(b,"title");
    author = readString(b,"author");
    ans = readString(b,"text");
    break;
  }    
}

void Card::response(const char r[]) {
  ans = new char[length(r)];
  copy(r,ans);
}

void Card::insert(Database* db, char user[]) {
  BSONObj a;
  switch(type) {
  case standard:  
    a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review);
    break;
  case poe:
    a = BSON("user" << user << "title" << prompt << "author" << author << "text" << ans << "type" << type << "next_review" << next_review);
    break;
  }
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

void Card::review(Print* pr) {
  p = pr;
  p->cls();
  p->print(prompt);
  review_state = 0;
}

int Card::review_msg(char c) {
  char a[200];
  sprintf(a,"rev_msg called with %c and review_state %i",c,review_state);
  log(a);
  if (review_state == 0) {
    p->cls();
    p->print(ans);
    review_state = 1;
    return 0;
  } else if (review_state == 1) {
    return grade(c); // If 1, cycle. If 2, quit.
  } 
}

int Card::grade(char c) {
  // Convert the grade.
  int a = -1;
  switch (c) {
  case '1':
    a = 1; break;
  case '2':
    a = 2; break;
  case '3':
    a = 3; break;
  case '4':
    a = 4; break;
  case '5':
    a = 5; break;
  }
  // Update the review time if valid grade given
  if (a != -1)
    updateTime(a);

  return (a == -1) + 1;
}

// Determine when the next review should be based on the grade
void Card::updateTime(int g) {
  int t = time(null);
  switch (g) {
  case 1:
    t += 10; break;
  case 2:
    t += 60; break;
  case 3:
    t += 60*30; break;
  case 4:
    t += 60*60*10; break;
  case 5:
    t += 60*60*24*2; break;
  }
  log("a");
  log(t);
  //fprintf(stderr,"%i time, not going to work",t);
  db->update("memory.data",
             BSON("_id" << id),
             BSON("next_review" << t));
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
