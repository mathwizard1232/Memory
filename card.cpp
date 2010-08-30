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

void Card::unlock(std::string id) {
  db->update("memory.data", id, BSON("active" << true));
}

void Card::front(std::string id, int offset) {
  db->update("memory.data", id, BSON("next_review" << (int)time(null) + offset));
}

Card::Card(const char p[])
  :type(standard), next_review(time(null)), active(true)
{
  prompt = new char[length(p)+1];
  copy(p,prompt);
}

Card::Card(const char p[], const char a[], const string u1, const string u2)
  :type(sequential), next_review(time(null)), components(), active(false) // Don't automatically activate sequential. Manually activate the first element.
{
  copy_leak(p,prompt);
  copy_leak(a,ans);
  unlock1 = u1;
  unlock2 = u2;
}

Card::Card(BSONObj b, Database* d) {
  this->db = d;
  BSONElement i;
  b.getObjectID(i);
  id = i.__oid().str();
  review_state = 0;
  int a = readInt(b,"type");
  type = a;
  next_review = readInt(b,"next_review");
  active = (bool) readInt(b,"active");
  last_interval = readInt(b,"last_interval");
  if (last_interval < 0) {
    last_interval = 0;
    previous_success = 0;
  } else {
    int tmp = readInt(b,"previous_success");
    if (tmp < 0) { // Must have only gotten 3s. Treat as no previous success
      previous_success = false;
    } else {
      previous_success = (bool) tmp;
    }
  }
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

  if ((type == sequential) || (type == poe)) {
    if (b.hasField("components")) { // Ignore atomic sequentials
      BSONObj arr = b.getObjectField("components");
      components = db->b_arr(arr);
    }
  }
}

void Card::setComponents(const string id, const vector<string> comp) {
  db->update("memory.data",id,"components",comp);
}

void Card::response(const char r[]) {
  ans = new char[length(r)];
  copy(r,ans);
}

string Card::insert(Database* d, char u[]) {
  Card::db = d; // Set the database connection for this Card.
  copy_leak(u,user);
  BSONObj a;
  string id, decomp;
  vector<string> subparts;
  switch(type) {
  case sequential:
    a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active << "unlock1" << unlock1 << "unlock2" << unlock2);
    id = Card::db->insert("memory.data",a);
    // If this can be decomposed, do so. Then return the first line to start the unlocking. Otherwise, simply return the id of the insert.
    decomp = decompose(id,subparts);
    if (decomp == "") {
      return id;
    } else {
      setComponents(id, subparts);
      return decomp;
    }
    break;    
  case standard:  
    a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active);
    return Card::db->insert("memory.data",a);
    break;
  case poe:
    // Don't start reviewing the full poem until entirely memorized.
    active = false;
    a = BSON("user" << user << "title" << prompt << "author" << author << "text" << ans << "type" << type << "next_review" << next_review << "active" << active);
    string id = Card::db->insert("memory.data",a); // Insert and get the id.
    decomp = decompose(id,subparts);
    setComponents(id,subparts);
    return decomp; // Break down into stanzas and insert them. Should return the first element (already activated by default. Could later do crazy stuff.)
    break;
  }
  
}

// Break a card down into component parts.
// These will then be memorized in order and after completed, this card will be reviewed.
// Returned value is the first id.
// Push the id of each component into parts. Don't touch current contents.
std::string Card::decompose(string id, vector<string>& parts) {
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
    parts.push_back(id);
    for (int i = stanzas.size()-2; i > 0; i--) {
      sprintf(p,"In %s, stanza following:\n%s",prompt,stanzas[i-1].c_str());
      length(stanzas[i].c_str());
      stanza = new Card(p,stanzas[i].c_str(),id);
      id = stanza->insert(db,user);
      parts.push_back(id);
    }
    sprintf(p,"In %s, give the first stanza.",prompt);
    stanza = new Card(p,stanzas[0].c_str(),id);
    id = stanza->insert(db,user);
    parts.push_back(id);
    // Make the first line active
    unlock(id);
    return id;
    break;
  case sequential:
    if (find(ans,'\n') != -1) { // If a sequential answer has multiple lines, break them up
      vector<string> lines = split(ans,"\n");
      char p[1000];
      for (int i = lines.size()-1; i>=0; i--) {
        sprintf(p,"%s\n\n",prompt);
        for (int j = 0; j < i; j++) {
          sprintf(p,"%s%s\n",p,lines[j].c_str());
        }
        sprintf(p,"%sWhat is the next line?",p);
        line = new Card(p,lines[i].c_str(),id);
        sprintf(p,"%s\n%s",p,lines[lines.size()-i-1].c_str());
        id = line->insert(db,user);
        parts.push_back(id);
      }
      return id; // Return the id of the first line
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

using mongo::JsonStringFormat;
// Determine when the next review should be based on the grade
void Card::updateTime(int g) {
  int t = time(null);
  int interval;
  switch (g) {
  case 1:
    interval = 10; break;
  case 2:
    interval = 60; break;
  case 3:
    interval = 60*30; break;
  case 4:
    interval = 60*60*10; break;
  case 5:
    interval = 60*60*24*2; break;
  }
  if (last_interval != 0) {
    // If we repeated a category, move further along it. Makes less difference for 1&2, since will usually be earliest possible anyhow.
    if ((g <= 2) && (!previous_success)) {
      interval = last_interval - 60;
    } else if ((g >= 4) && (previous_success)) {
      interval = last_interval;
      if (g == 4) {
        interval *= 3;
      } else {
        interval *= 8;
      }
    }
  }
  log("Next interval for: " + id);
  log(interval);
  log("After grade of " + i_str(g));
  log("And previous " + i_str(previous_success) + " and prev_int " + i_str(last_interval));

  if (g >= 4) {
    previous_success = true;
  } else if (g <= 2) {
    previous_success = false;
  }
  t += interval;
  db->update("memory.data",id,BSON("next_review" << t << "previous_success" << previous_success << "last_interval" << interval));

  // If this is a sequential card and a "good" rating was given (>=4), unlock next card.
  if ((type == sequential) && (g>=4)) {
    unlock(unlock1);
    // If there is a second to unlock, do so.
    if (unlock2 != "") {
      unlock(unlock2);
    }
  }

  // If this card is composed of multiple elements, and a grade == 1 was given, move the elements to the front and this slightly behind.
  if (components.size() > 0) {
    if (g == 1) {
      for (int i = components.size() - 1; i >= 0; i--) {
        front(components[i],components.size() - i);
      }
    } else if (g == 2) { // Move any element which is below the mean next_review time to the front.
      long sum = 0;
      for (int i = 0; i < components.size(); i++) {
        sum += db->getInt("memory.data",components[i],"next_review");
      }
      int mean = (sum / components.size()) + 1;
      for (int i = components.size() - 1; i >= 0; i--) {
        if (db->getInt("memory.data",components[i],"next_review") <= mean) {
          front(components[i],components.size() - i);
        }
      }
    } else if (g == 3) { // Move the element(s) which is to be reviewed the soonest to the front.
      int min = db->getInt("memory.data",components[0],"next_review");
      int temp;
      for (int i = 1; i < components.size(); i++) {
        temp = db->getInt("memory.data",components[i],"next_review");
        if (temp < min) { min = temp; }
      }

      for (int i = components.size() - 1; i >= 0; i--) {
        if (db->getInt("memory.data",components[i],"next_review") <= min) {
          front(components[i],components.size() - i);
        }
      }
    } // TODO: Eventually, might consider strengthening next_review or last_interval for components on that basis of a pass on the whole.
  }
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
