#include "poem.h"
#include "sequential.h"
#include <fstream>
using std::ifstream;
using std::stringstream;
using std::string;

Poem::Poem()
{
}

/* Poems must be plain ASCII text.
   First line: title
   Second line: "By $author"
   Third line: blank
   Rest: poem, extra newline between stanzas */
Poem::Poem(const char file[]) {
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

// Break a card down into component parts.
// These will then be memorized in order and after completed, this card will be reviewed.
// Returned value is the first id.
// Push the id of each component into parts. Don't touch current contents.
string Poem::decompose(string id, vector<string>& parts) {
  vector<string> stanzas;
  Card* stanza;
  
  Card* line;
  string stanza_id;

    stanzas = split(ans,"\n\n");
    char p[1000];
    //    sprintf(p,("In %s, stanza #" + i_str(stanzas.size()) + ", following:\n%s").c_str(),prompt,stanzas[stanzas.size()-2].c_str());
    //    stanza = new Sequential(p,stanzas[stanzas.size()-1].c_str(),id); // unlock poem when final stanza memorized
    // Insert the last stanza, which activates the whole poem
    // Save the id of the first element of the last stanza to be unlocked by the previous stanza
    //    id = stanza->insert(db,user,stanza_id);
    //    parts.push_back(stanza_id);
    for (int i = stanzas.size()-1; i > 0; i--) {
      sprintf(p,("In %s, stanza #" + i_str(i+1) + ", following:\n%s").c_str(),prompt,stanzas[i-1].c_str());
      //      length(stanzas[i].c_str());
      stanza = new Sequential(p,stanzas[i].c_str(),id);
      stanza->setCategory(cat);
      id = stanza->insert(db,user,stanza_id);
      parts.push_back(stanza_id);
    }
    sprintf(p,"In %s, give the first stanza.",prompt);
    stanza = new Sequential(p,stanzas[0].c_str(),id);
    id = stanza->insert(db,user,stanza_id);
    parts.push_back(stanza_id);
    // Make the first line active
    unlock(id);
    return id;
}

// This_id is the id of this [full] card inserted. The return value is the first [element] card to be activated for this block of memory.
// For a basic flashcard, these are identical.
string Poem::insert(Database* d, char u[], string& this_id) {
  Card::db = d; // Set the database connection for this Card.
  user = strdup(u);
  BSONObj a;
  string decomp;
  vector<string> subparts;

    // Don't start reviewing the full poem until entirely memorized.
    active = false;
    a = BSON("user" << user << "title" << prompt << "author" << author << "text" << ans << "type" << type << "next_review" << next_review << "active" << active << "category" << cat->toString());
    this_id = Card::db->insert("memory.data",a); // Insert and get the id.
    decomp = decompose(this_id,subparts);
    setComponents(this_id,subparts);
    return decomp; // Break down into stanzas and insert them. Should return the first element (already activated by default. Could later do crazy stuff.)

}

void Poem::finishRead(BSONObj b, Database* d) {
  prompt = strdup(readString(b,"title").c_str());
  author = strdup(readString(b,"author").c_str());
  ans = strdup(readString(b,"text").c_str());
  
  if (b.hasField("components")) { // Ignore atomic sequentials
    BSONObj arr = b.getObjectField("components");
    components = db->b_arr(arr);
  }
}
