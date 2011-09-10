// First draft refactoring finished 1/1/11.

#include "card.h"
#include "utility.h"
#include <fstream>
#include <string>
#include <boost/regex.hpp>
#include "time.h"

// Need subclasses for factory
#include "basic.h"
#include "sequential.h"
#include "poem.h"

using std::ifstream;
using std::stringstream;
using std::string;

Database* Card::db;
Print* Card::p;

void Card::unlock(std::string id) {
  db->update("memory.data", id, BSON("active" << true));
}

void Card::front(std::string id, int offset) {
  db->update("memory.data", id, BSON("next_review" << (int)time(null) + offset));
}

void Card::setDB(Database* d) {
  db = d;
}

Card::Card() 
  :type(nil), active(false), prompt(0), ans(0), user(0), author(0)
{
  cat = Category::all();
}

Card::~Card() {
  if (prompt) {
    free(prompt);
  }
  if (ans) {
    free(ans);
  }
  if (user) {
    free(user);
  }
  if (author) {
    free(author);
  }
}

Card* Card::CardFactory(BSONObj b, Database* d) {
  db = d;

  Card* c;
  switch (readInt(b,"type")) {
  case standard:
    c = new Basic();
    break;
  case sequential:
    c = new Sequential();
    break;
  case poe:
    c = new Poem();
    break;
  default:
    log("Unrecognized card type:");
    //1/0; // Bad form
    perror("Unrecognized card type.");
    //    log(b);
  };

  //  BSONElement i;
  //  b.getObjectID(i);
  //  c->id = i.__oid().str();
  c->id = readId(b);
  c->type = (CardType) readInt(b,"type");
  c->next_review = readInt(b,"next_review");
  c->active = (bool) readInt(b,"active");
  c->last_interval = readInt(b,"last_interval");
  if (c->last_interval < 0) { // There should be a more elegant way to detect strange or uninitialized last_intervals.
    c->last_interval = 0;
    c->previous_success = 0;
  } else {
    c->previous_success = readBool(b, "previous_success");
  }
  c->review_state = 0;

  if (b.hasField("category")) {
    c->cat = Category::find(readString(b,"category"));
  } else {
    c->cat = Category::all();
  }

  c->finishRead(b,d);
  return c;
}

void Card::finishRead(BSONObj b, Database* d) {
}

void Card::setComponents(const string id, const vector<string> comp) {
  db->update("memory.data",id,"components",comp);
}

// Made virtual since always overridden.
std::string Card::insert(Database* db, char user[], std::string& this_id) {
  perror("Card::insert should not be called; only the overridden descendents' versions.");
}

string Card::insert(Database* db, char u[]) {
  string temp;
  return insert(db,u,temp);
}

void Card::setCategory(Category* c) {
  cat = c;
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
  // Add category suffix
  char* a = (char*) malloc(sizeof(char)*(strlen(prompt) + cat->getSuffix().length()+2));
  sprintf(a,"%s %s",prompt,cat->getSuffix().c_str());
  log("Reviewing card.");
  log(id);
  log(cat->getSuffix());
  //  p->print(prompt,true);
  p->print(a);
  //  p->redraw();
  review_state = 0;
  free(a);
}

int Card::review_msg(char c) {
  if (review_state == 0) {
    p->cls();
    p->print(ans,true); // Change print.h to make this single line
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

// 2/11/11: Refactoring this function is my next priority. Functional change will be preventing time overflows; aesthetically will
// work on making this two or three functions in the end and more consistent flow.
using mongo::JsonStringFormat;
// Determine when the next review should be based on the grade
// We should eventually use more information than just grade and previous_success.
// We should also consider the actual elapsed time since the last repetition at a minimum.
void Card::updateTime(int g) {
  double t = time(null); // Retrieve the current time. Use a double to deal with y2k-esque overflow.
  double interval;

  const int MIN = 60;
  const int HOUR = 60 * MIN;
  const int DAY = HOUR * 24;
  const int YEAR = DAY * 365;

  // Select the "base" time interval for the given grade.
  // We will consider a 3 as suggesting another look to strengthen the knowledge.
  // 1/2 suggests not yet learned, review ASAP, essentially in arbitrary order.
  // 4/5 The card has been learned. If a 4, review again in 10 hours. If 5, 2 days.
  // These are arbitrary values which seemed reasonable.
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

  // If we have repeated success, then lengthen interval.
  if (last_interval != 0) { // If we have previously worked with this card
    if ((g >= 4) && (previous_success)) { // and we know it now and knew it last
      interval = max(last_interval, interval); // then take the previous interval (or default if higher)
      if (g == 4) {
        interval *= 3; // and lengthen it. More for a 5 than a 4, but significantly for both.
      } else {
        interval *= 6;
      }
    }
  }

  // Now, ensure that the interval is no longer than 2 years.
  interval = min(interval,2*YEAR); // Again, arbitrary value that seems reasonable at the time. (2/18/11)
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

  // If we're going to break this card down, don't schedule this card until after the components are reviewed.
  if ((components.size() > 0) && (g == 1)) { 
    t += components.size();
  }
  db->update("memory.data",id,BSON("next_review" << t << "previous_success" << previous_success << "last_interval" << interval));

  polyUpdate(g);
  // TODO: Abstract below into separate functions. If not significantly reduced in size, consider eventually separate class structure.

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

void Card::polyUpdate(int a)
{
}

std::string Card::decompose(std::string id,std::vector<std::string>& parts)
{
  return (string)0;
}

using std::ofstream;
using std::endl;
void Card::write(ofstream &o) {
  o << "q: ";
  o << prompt;
  o << endl;
  o << "a: " << ans << endl;
}
