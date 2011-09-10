#include "memory.h"
#include "utility.h"

// Subtypes of cards
// note: so far sequential is only triggered by poem
#include "basic.h"
#include "poem.h"

enum status {main_menu,menu,dynamic}; // Legacy type.

// These functions are for when there is no current state or when the current message function,
// rather than using the dynamic system are using the current single function.
int Memory::nullcmessage(char c) {
  return 0;
}

int Memory::nullstrmessage(const char c[]) {
  return 0;
}

Memory::Memory()
  :started(false),cmessagep(&Memory::nullcmessage),strmessagep(&Memory::nullstrmessage)
{  
  card = null;
  cat = Category::all();
  users = db.users();
}

Memory::~Memory()
{
  int i;
  for (i = 0; i < users.size(); i++) {
    delete[] users[i];
  }
}

void Memory::set_printer(Print* in) {
  p = in;
  switch_to_main_menu();
}

void Memory::display_main_menu() {
  p->print("Welcome to memory.");
  p->print("(L)ogin");
  p->print("Create a (n)ew user");
}

void Memory::switch_to_main_menu() {
  cmessagep = &Memory::main_menu_c;
  p->cls();
  display_main_menu();
}

static bool start = false;
void Memory::display_menu() {
  if (!start) { // For some reason, this crashes from inside Memory constructor.
    log("Starting to load categories.");
    Category::load(&db);
    log("Categories loaded.");
    start = true;
  }

  if (!started) {
    char out[200];
    sprintf(out,"Welcome, %s",user);
    p->print(out);
    started = true;
  }
  p->print("(A)dd new data");
  p->print("(R)eview information");
  p->print("(T)est knowledge");
  p->print("(M)anage knowledge");
}

void Memory::switch_to_menu() {
  cmessagep = &Memory::menu_c;
  p->cls();
  display_menu();
}

int Memory::create_user_str(const char str[]) {
  db.add_user(str);
  user = strdup(str);
  switch_to_menu();
  return 1;
}

int Memory::which_add_c(char c) {
  if (charcmp(c,'s')) {
    cmessagep = &Memory::add_c;
    strmessagep = &Memory::add_str;
    substate = 0;
    add();
  } else if (charcmp(c,'p')) {
    cmessagep = &Memory::poem_c;
    strmessagep = &Memory::poem_str;
    poem();
  } else if (charcmp(c,'b')) {
    switch_to_menu();
    return 0;
  }
  return 1;
}

int Memory::review_c(char c) {
  if (substate == 0) {
    substate = card->review_msg(c);
    if (substate == -1) { // unrecognized input c, don't switch states
      substate = 0;
    }
  }
  if (substate == 1) {
    review();
    substate = 0;
  } else if (substate == 2) {
    switch_to_menu();
  }
  return 0;
}

int Memory::menu_c(char c) {
  if (charcmp(c,'a')) {
    // Start adding a new piece of information
    cmessagep = &Memory::which_add_c;
    substate = 0;
    //add();
    which_add();
  } else if (charcmp(c,'r')) {
    cmessagep = &Memory::review_c;
    substate = 0;
    review();
  } else if (charcmp(c,'m')) {
    cmessagep = &Memory::manage_c;
    manage();
  }
  return 0;
}

int Memory::main_menu_c(char c) { 
  if (charcmp(c,'l')) {
    return login();
  }
  else if (charcmp(c,'n')) {
    strmessagep = &Memory::create_user_str;
    new_user();
    return 1;
  }
  return 0; // If there is an invalid entry, wait for a valid one.
}

int Memory::add_c(char c) {
  if (yn(c)) {
    if (yes(c)) {
      card->insert(&db,user);
      free(card);
      card = null;
      substate = 0;
      add();
      return 1;
    }
    else {
      free(card);
      switch_to_menu();
      return 0;
    }
  }
}

int Memory::poem_c(char c) {
  if (yn(c)) {
    if (yes(c)) {
      card->insert(&db,user);
      p->cls();
      p->print("Poem added.");
      cmessagep = &Memory::menu_c;
      display_menu();
    } else {
      switch_to_menu();
    }
  }
  return 0;
}

int Memory::press_key_c(char c) {
  resume();
  return 0;
}

int Memory::login_c(char c) {
  char a[2];
  a[0] = c;
  a[1] = '\0';
  int b = atoi(a);
  if (b) {
    user = strdup(users[b-1]);
    switch_to_menu();
  }
  else {
    switch_to_main_menu();
  }
  return 0;
}

int Memory::login_str(const char str[]) {
  char* tmp = strdup(str);
  int id = cstr_i(tmp);
  free(tmp);

  log("Logging in as user:");
  log(id);
  log((int) users.size());
  if ((id < 1) || (id > users.size())) {
    // Invalid input. Go back to the main menu.
    switch_to_main_menu();
  } else {
    log("logging in as:");
    //    user = new char[strlen(users[id-1])+1];
    //    copy((char*) users[id-1], user);
    //    copy_leak((char*)users[id-1],user);
    user = strdup(users[id-1]);
    switch_to_menu();
    log(user);
  }
  log("done");
  return 1;
}

// If 1 is returned, takes in a string next.
// If 0, continues to take characters.
// Create new message functions. Then message will simply call the relevant function.
int Memory::message(char c) {
  // Handle the character if logical
  if (c != '\0' && (int)c != -1) {
    return (this->*cmessagep)(c);
  }
  return 0;
}

void Memory::review() {
  if (card) {
    delete card;
  }
  card = db.next_review(user);
  if (card == null) {
    cmessagep = &Memory::menu_c;
    p->print("Please add a flashcard before reviewing.");
  }
  else
    card->review(p);
}

bool Memory::yn(char c) {
  return (charcmp(c,'y') || charcmp(c,'n'));
}

bool Memory::yes(char c) {
  return charcmp(c,'y');
}

void Memory::poem() {
  if (substate == 0) {
    p->cls();
    p->print("Enter filename: ");
    substate = 1;
  } else {
    p->cls();
    p->print("Is the following correct?");
    card->print(p);
  }
}

void Memory::which_add() {
  p->cls();
  p->print("Select method of input.",true);
  p->print("(S)imple flashcard",true);
  p->print("(P)oem from file",true);
  p->print("(B)ack");
}

void Memory::add() {
  switch (substate) {
  case 0:
    p->cls();
    p->print("Please enter the prompt (blank to quit). ");
    substate = 1;
    break;
  case 1:
    p->cls();
    p->print("Enter the correct response. ");
    substate = 2;
    break;
  case 2:
    p->cls();
    p->print("Is the following correct? (y/n)");
    card->print(p);
    substate = 3;
  }
}

int Memory::add_str(const char str[]) {
  if (substate == 1) {
    if (str[0] == '\0') {
      switch_to_menu();
      return 1;
    } else {
      card = new Basic(str);
      card->setCategory(cat);
      add();
      return 0;
    }
  } else {
    Basic* p = (Basic*) card;
    p->response(str);
    add();
    return 1;
  }
}

int Memory::poem_str(const char str[]) {
  card = new Poem(str);
  card->setCategory(cat);
  poem();
  return 1;
}

int Memory::message(const char str[]) {
  return (this->*strmessagep)(str);
}

void Memory::continue_at(int state) {
  cmessagep = &Memory::press_key_c;
  resume_state = state;
  p->print("Press 'q' to exit or any other key to continue.");
}

void Memory::resume() {
  switch(resume_state) {
  case main_menu:
    switch_to_main_menu();
    break;
  case menu:
    switch_to_menu();
    break;
  }
}

// return 1 if >= 10 users and start complex input
int Memory::login() {
  int i;
  for (i = 0; i < users.size(); i++) {
    delete[] users[i];
  }
  users = db.users();

  cmessagep = &Memory::login_c;
  strmessagep = &Memory::login_str;

  p->cls();
  if (users.size() == 0) {
    p->print("Please create a user first.");
    continue_at(main_menu); // Wait for key, then ...
    return 0;
  }
  else if (users.size() >= 10) {
    //p->print("Multi-character login now needs to be coded. ~1 hour. Please pay the original programmer if you would like this done.");
    //    p->print("
    p->print("Please select from the following users and press enter when done.");
    p->list(users);
    //    p->newline();
    p->print("\n ");
    return 1;
  }
  else {
    p->print("Please select from the following users or press 'b' to return to the main menu.");
    p->list(users);
    return 0;
  }
}

void Memory::new_user() {
  p->cls();
  p->print("What would you like to be called? ");
}

void Memory::manage() {
  p->cls();
  p->print("(C)ategories");
  p->print("(D)ump all data to a flat file.");
  p->print("(B)ack to the main menu.");
}

int Memory::manage_c(char c) {
  switch (tolower(c)) {
  case 'd': dump(); break;
  case 'b': switch_to_menu(); break;
  case 'c': cmessagep = &Memory::categories_c; strmessagep = &Memory::categories_str; categories(); break;
  }
  return 0;
}


void Memory::categories() {

  p->cls();
  p->printf("Current category: %s",cat->toString().c_str());
  if (!cat->rootp()) {
    p->print("Move to (p)arent");
  }
  // List children
  vector<Category*> children = cat->getChildren();
  if (children.size() > 10) {
    log("Memory::categories problem; numerical input must now be fixed.");
    p->print("Numerical input greater than 10 not yet implemented. Please contact the author.");
  }
  vector<std::string> names;
  for (int i = 0; i < children.size(); i++) {
    names.push_back(children[i]->toString());
  }
  p->list(names);    
  
  p->print("Make new (c)hild");
  p->print("");
  p->printf("Current suffix: %s",cat->getSuffix().c_str());
  p->print("(N)ew suffix");
  p->print("");
  p->print("(B)ack");
}

int Memory::categories_c(char c) {
  switch (tolower(c)) {
  case 'p': cat = cat->getParent(); categories(); break;
  case 'c': substate = 1; child(); return 1; // Go to a long input, yet to be coded.
  case 'n': substate = 2; suffix(); return 1; // ""
  case 'b': cmessagep = &Memory::manage_c; manage(); break;
  }

  if ((c >= '0') && (c <= '9')) {
    int i = (int)c - (int)'0';
    if (i == 0)
      i = 10;

    vector<Category*> children = cat->getChildren();
    if (i <= children.size()) {
      cat = children[i-1];
      categories();
    }
  }

  return 0;
}

void Memory::child() {
  p->cls();
  p->print("Enter the name of the new child category (blank to cancel): ");
}

void Memory::suffix() {
  p->cls();
  p->print("Enter the new suffix for this category: ");
}

int Memory::categories_str(const char* c) {
  if (substate == 1) {
    // Create the new child
    if (c != "") {
      cat = cat->makeChild(c);
      cat->insert(&db);
    }
  } else if (substate == 2) {
    // Change the suffix
    cat->setSuffix(c);
    cat->update(&db);
  }
  categories();
  return 1;
}

void Memory::dump() {
  ofstream out;
  out.open("dump.txt");
  db.dump(out,user);
}
