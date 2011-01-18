#include "sequential.h"
using std::string;

Sequential::Sequential()
{
}

Sequential::Sequential(const char p[], const char a[], const string u1, const string u2)
{
  type = sequential;
  next_review = time(null);
  active = false; // Don't automatically activate sequential. Manually activate the first element.

  copy_leak(p,prompt);
  copy_leak(a,ans);
  unlock1 = u1;
  unlock2 = u2;
}

// Break a card down into component parts.
// These will then be memorized in order and after completed, this card will be reviewed.
// Returned value is the first id.
// Push the id of each component into parts. Don't touch current contents.
std::string Sequential::decompose(string id, vector<string>& parts) {
  vector<string> stanzas;
  Card* stanza;
  
  Card* line;
  string stanza_id;

    if (find(ans,'\n') != -1) { // If a sequential answer has multiple lines, break them up
      vector<string> lines = split(ans,"\n");
      char p[10000];
      for (int i = lines.size()-1; i>=0; i--) {
        sprintf(p,"%s\n\n",prompt);
        for (int j = 0; j < i; j++) {
          sprintf(p,"%s%s\n",p,lines[j].c_str());
        }
        sprintf(p,"%sWhat is the next line?",p);
        line = new Sequential(p,lines[i].c_str(),id);
        line->setCategory(cat);
        sprintf(p,"%s\n%s",p,lines[lines.size()-i-1].c_str());
        id = line->insert(db,user);
        parts.push_back(id);
      }
      return id; // Return the id of the first line
    } // Otherwise, no action
      return "";
}

// This_id is the id of this [full] card inserted. The return value is the first [element] card to be activated for this block of memory.
// For a basic flashcard, these are identical.
string Sequential::insert(Database* d, char u[], string& this_id) {
  Card::db = d; // Set the database connection for this Card.
  copy_leak(u,user);
  BSONObj a;
  string decomp;
  vector<string> subparts;

  a = BSON("user" << user << "prompt" << prompt << "response" << ans << "type" << type << "next_review" << next_review << "active" << active << "unlock1" << unlock1 << "unlock2" << unlock2 << "category" << cat->toString());
    this_id = Card::db->insert("memory.data",a);
    // If this can be decomposed, do so. Then return the first line to start the unlocking. Otherwise, simply return the id of the insert.
    decomp = decompose(this_id,subparts);
    if (decomp == "") {
      return this_id;
    } else {
      setComponents(this_id, subparts);
      return decomp;
    }
}

void Sequential::finishRead(BSONObj b, Database* d) {
   unlock1 = b.getStringField("unlock1");
   unlock2 = b.getStringField("unlock2");
   prompt = readString(b,"prompt");
   ans = readString(b,"response");

   if (b.hasField("components")) { // Ignore atomic sequentials
     BSONObj arr = b.getObjectField("components");
     components = db->b_arr(arr);
   }
}

void Sequential::polyUpdate(int g) {
  // If a "good" rating was given (>=4), unlock next card.
  if (g>=4) {
    unlock(unlock1);
    front(unlock1); // Put card at current time rather than creation time.
    // If there is a second to unlock, do so.
    if (unlock2 != "") {
      unlock(unlock2);
      front(unlock2);
    }
  }
}
