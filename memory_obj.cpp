#include "memory.h"
#include "utility.h"

enum status {main_menu = 0, // Initial screen 
             logi = 1, // Typing more than one character for login, >= 10 users
             menu = 2, // Logged in main menu
             press_key = 3, // Waiting for keypress to continue
             create_user = 4, //adding a new user
             ad = 5, //adding new information (simple)
             poe = 6, // adding a new poem
             which_ad = 7, // selecting add method
             revie = 8 // review a card
};
  

Memory::Memory()
  :state(0),started(false)
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
  state = 0;
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
}

void Memory::switch_to_menu() {
  state = menu;
  p->cls();
  display_menu();
}

// If 1 is returned, takes in a string next.
// If 0, continues to take characters.
int Memory::message(char c) {
  int a = (int)c;

  char* b = new char[4];
  sprintf(b,"%i",a);
  // Handle the character if logical
  if (c != '\0' && a != -1) {
    // Lol @ switch statements
    char* a = new char[2];
    int b;
    switch(state) {
    case main_menu:
      if (charcmp(c,'l')) {
        return login();
      }
      else if (charcmp(c,'n')) {
        state=create_user;
        new_user();
        return 1;
      }
      else {
        p->print(&c);
      }
      break;
    case press_key:
      resume();
      break;
    case logi:
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
      break;
    case menu:
      if (charcmp(c,'a')) {
        // Start adding a new piece of information
        state = which_ad;
        substate = 0;
        //add();
        which_add();
        return 0;
      } else if (charcmp(c,'r')) {
        state = revie;
        substate = 0;
        review();
      }
      break;
    case which_ad:
      if (charcmp(c,'s')) {
        state = ad;
        substate = 0;
        add();
        return 1;
      } else if (charcmp(c,'p')) {
        state = poe;
        poem();
        return 1;
      }
      break;
    case ad:
      if (yn(c)) {
        if (yes(c)) {
          card->insert(&db,user);
          substate = 0;
          add();
          return 1;
        }
        else {
          switch_to_menu();
        }
      }
      break;
    case poe:
      if (yn(c)) {
        if (yes(c)) {
          log("Insert call");
          card->insert(&db,user);
          log("Inserted");
          p->cls();
          p->print("Poem added.");
          state = menu;
          display_menu();
        } else {
          switch_to_menu();
        }
      }
      break;
    case revie:
      if (substate == 0) {
        substate = card->review_msg(c);
      }
      if (substate == 1) {
        review();
        substate = 0;
      } else if (substate == 2) {
        switch_to_menu();
      }
      break;
    }
  }
  return 0;
}

void Memory::review() {
  card = db.next_review(user);
  if (card == null) {
    state = menu;
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

int Memory::message(char str[]) {
  switch (state) {
  case create_user:
    db.add_user(str);
    user = str;
    switch_to_menu();
    return 1;
    break;
  case ad:
    if (substate == 1) {
      if (str[0] == '\0') {
        switch_to_menu();
        return 1;
      } else {
        card = new Card(str);
        add();
        return 0;
      }
    } else {
      card->response(str);
      add();
      return 1;
    }
    break;
  case poe:
    card = new Card("making poem");
    card->poem(str);
    poem();
    return 1;
  }
}

void Memory::continue_at(int stat) {
  state = press_key;
  resume_state = stat;
  p->print("Press 'q' to exit or any other key to continue.");
}

void Memory::resume() {
  state = resume_state;
  switch(state) {
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
  p->cls();
  if (users.size() == 0) {
    p->print("Please create a user first.");
    continue_at(main_menu); // Wait for key, then ...
    return 0;
  }
  else if (users.size() >= 10) {
    p->print("Multi-character login now needs to be coded. ~1 hour. Please pay the original programmer if you would like this done.");
    state = logi;
    return 1;
  }
  else {
    p->print("Please select from the following users or press 'b' to return to the main menu.");
    p->list(users);
    state = logi;
    return 0;
  }
}

void Memory::new_user() {
  p->cls();
  p->print("What would you like to be called? ");
}
