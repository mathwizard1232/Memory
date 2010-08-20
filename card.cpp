#include "card.h"
#include "utility.h"
#include <fstream>
#include <string>
#include <boost/regex.hpp>
#include "time.h"

using std::ifstream;
using std::stringstream;
using std::string;

Database* Card::db;

Card::Card(const char p[])
  :type(standard), next_review(time(null)), active(true)
{
  prompt = new char[length(p)+1];
  copy(p,prompt);
}

Card::Card(const char p[], const char a[], const string u1, const string u2)
  :type(sequential), next_review(time(null)), active(true)
{
  copy_leak(p,prompt);
  copy_leak(a,ans);
  unlock1 = u1;
  unlock2 = u2;
}

Card::Card(BSONObj b, Database* d) {
  this->db = d;
  b.getObjectID(id);
  log(id);
  review_state = 0;
  int a = readInt(b,"type");
  log(a);
  type = a;
  next_review = readInt(b,"next_review");
  active = (bool) readInt(b,"active");
  switch (type) {
  case sequential:
    unlock1 = b.getStringField("unlock1");
    unlock2 = b.getStringField("unlock2");
    // Fall through to standard for rest
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

string Card::insert(Database* d, char u[]) {
  Card::db = d; // Set the database connection for this Card.
  copy_leak(u,user);
  BSONObj a;
  switch(type) {
  case sequential:
    a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active << "unlock1" << unlock1 << "unlock2" << unlock2);
    return Card::db->insert("memory.data",a);
    break;    
  case standard:  
    a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active);
    return Card::db->insert("memory.data",a);
    break;
  case poe:
    // Don't start reviewing the full poem until entirely memorized.
    a = BSON("user" << user << "title" << prompt << "author" << author << "text" << ans << "type" << type << "next_review" << next_review << "active" << active);
    string id = Card::db->insert("memory.data",a); // Insert and get the id.
    log("Inserted main poem.");
    return decompose(id); // Break down into stanzas and insert them. Should return the first element (already activated by default. Could later do crazy stuff.)
    break;
  }
  
}

// Break a card down into component parts.
// These will then be memorized in order and after completed, this card will be reviewed.
// Returned value is the first id.
std::string Card::decompose(string id) {
  vector<string> stanzas;
  Card* stanza;
  
  Card* line;
  switch (type) {
  case poe:
    stanzas = split(ans,"\n\n");
    char p[1000];
    sprintf(p,"In %s, stanza following:\n%s",prompt,stanzas[stanzas.size()-2].c_str());
    stanza = new Card(p,stanzas[stanzas.size()-1].c_str(),id); // unlock poem when final stanza memorized
    // Insert the last stanza, which activates the whole poem
    // Save the id of the first element of the last stanza to be unlocked by the previous stanza
    id = stanza->insert(db,user);
    for (int i = stanzas.size()-2; i > 0; i--) {
      sprintf(p,"In %s, stanza following:\n%s",prompt,stanzas[i-1].c_str());
      length(stanzas[i].c_str());
      stanza = new Card(p,stanzas[i].c_str(),id);
      id = stanza->insert(db,user);
    }
    sprintf(p,"In %s, give the first stanza.",prompt);
    stanza = new Card(p,stanzas[0].c_str(),id);
    id = stanza->insert(db,user);
    // Make the first line active
    unlock(id);
    return id;
    break;
  case sequential:
    if (find(prompt,'\n') != -1) { // If a sequential prompt has multiple lines, break them up
      vector<string> lines = split(ans,"\n");
      char p[1000];
      char pr[1000];
      sprintf(p,"%s\n",prompt);
      for (int i = lines.size()-1; i>=0; i++) {
        sprintf(pr,"%s\nWhat is the next line?",p);
        line = new Card(pr,lines[i].c_str(),id);
        sprintf(p,"%s\n%s\n",p,lines[i].c_str());
        id = line->insert(db,user);
      }
    } // Otherwise, no action
      return "";
  } 
}

void Card::print(Print* p) {
  switch (type) {
  case standard:
    p->printf("Prompt: %s",prompt);
    p->printf("Response: %s",ans);
    break;
  case poe:
    p->printf("Title: %s",prompt,true);
    p->printf("Author: %s",author,true);
    p->printf("Text: %s",ans,true);
    p->redraw();
    break;
  };
}

void Card::review(Print* pr) {
  p = pr;
  p->cls();
  p->print(prompt,true);
  p->redraw();
  review_state = 0;
}

int Card::review_msg(char c) {
  if (review_state == 0) {
    p->cls();
    p->print(ans,true);
    p->redraw();
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
