#include "memory.h"
#include "utility.h"

// Subtypes of cards
#include "basic.h"
#include "poem.h"

enum status {main_menu = 0, // Initial screen 
             logi = 1, // Typing more than one character for login, >= 10 users
             menu = 2, // Logged in main menu
             press_key = 3, // Waiting for keypress to continue
             create_user = 4, //adding a new user
             ad = 5, //adding new information (simple)
             poe = 6, // adding a new poem
             which_ad = 7, // selecting add method
             revie = 8, // review a card
             dynamic = 9 // use the message pointers rather than state
};

// These functions are for when there is no current state or when the current message function,
// rather than using the dynamic system are using the current single function.
int Memory::nullcmessage(char c) {
  return 0;
}

int Memory::nullstrmessage(char c[]) {
  return 0;
}

Memory::Memory()
  :started(false),cmessagep(&Memory::nullcmessage),strmessagep(&Memory::nullstrmessage)
{  
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
  //  state = main_menu;
  state = dynamic;
  cmessagep = &Memory::main_menu_c;
  p->cls();
  display_main_menu();
}

void Memory::display_menu() {
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
  //  state = menu;
  state = dynamic;
  cmessagep = &Memory::menu_c;
  p->cls();
  display_menu();
}

int Memory::create_user_str(char str[]) {
  db.add_user(str);
  user = str;
  switch_to_menu();
  return 1;
}

int Memory::which_add_c(char c) {
  if (charcmp(c,'s')) {
    //state = ad;
    cmessagep = &Memory::add_c;
    strmessagep = &Memory::add_str;
    substate = 0;
    add();
  } else if (charcmp(c,'p')) {
    //state = poe;
    cmessagep = &Memory::poem_c;
    strmessagep = &Memory::poem_str;
    poem();
  }
  state = dynamic;
  return 1;
}

int Memory::review_c(char c) {
  if (substate == 0) {
    substate = card->review_msg(c);
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
    //state = which_ad;
    cmessagep = &Memory::which_add_c;
    state = dynamic;
    substate = 0;
    //add();
    which_add();
  } else if (charcmp(c,'r')) {
    //        state = revie;
    state = dynamic;
    cmessagep = &Memory::review_c;
    substate = 0;
    review();
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
    state = dynamic;
    return 1;
  }
  return 0; // If there is an invalid entry, wait for a valid one.
}

int Memory::add_c(char c) {
  if (yn(c)) {
    if (yes(c)) {
      card->insert(&db,user);
      substate = 0;
      add();
      return 1;
    }
    else {
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
      //state = menu;
      state = dynamic;
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
  // This is sloppy because it contains ideas about how to do a clever multikey entry for login.
  char* a = new char[2];
  int b;
  a[0] = c;
  a[1] = '\0';
  b = atoi(a);
  if (b) {
    user = new char[length((char*) users[b-1])+1];
    copy((char*) users[b-1], user);
    switch_to_menu();
  }
  else {
    switch_to_main_menu();
  }
  return 0;
}

// If 1 is returned, takes in a string next.
// If 0, continues to take characters.
// Create new message functions. Then message will simply call the relevant function.
int Memory::message(char c) {
  int a = (int)c;

  char* b = new char[4];
  sprintf(b,"%i",a);
  // Handle the character if logical
  if (c != '\0' && a != -1) {
    return (this->*cmessagep)(c);
  }
  return 0;
}

void Memory::review() {
  card = db.next_review(user);
  if (card == null) {
    //state = menu;
    state = dynamic;
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
  p->print("Select method of input.");
  p->print("(S)imple flashcard");
  p->print("(P)oem from file");
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

int Memory::add_str(char str[]) {
  if (substate == 1) {
    if (str[0] == '\0') {
      switch_to_menu();
      return 1;
    } else {
      card = new Basic(str);
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

int Memory::poem_str(char str[]) {
  card = new Poem(str);
  poem();
  return 1;
}

int Memory::message(char str[]) {
  return (this->*strmessagep)(str);
}

void Memory::continue_at(int state) {
  //  state = press_key;
  state = dynamic;
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
  users = db.users();

  cmessagep = &Memory::login_c;
  state = dynamic;
  p->cls();
  if (users.size() == 0) {
    p->print("Please create a user first.");
    continue_at(main_menu); // Wait for key, then ...
    return 0;
  }
  else if (users.size() >= 10) {
    p->print("Multi-character login now needs to be coded. ~1 hour. Please pay the original programmer if you would like this done.");
    //    state = logi;
    return 1;
  }
  else {
    p->print("Please select from the following users or press 'b' to return to the main menu.");
    p->list(users);
    //    state = logi;
    return 0;
  }
}

void Memory::new_user() {
  p->cls();
  p->print("What would you like to be called? ");
}
