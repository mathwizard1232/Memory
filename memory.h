#ifndef MEMORY_OBJ
#define MEMORY_OBJ

#include "print.h"
#include "database.h"
#include "card.h"

class Memory {
 public:
  Memory();

  // Handle an incoming character.
  // If anything other than 0 returned, switch to line input
  int message(char c);

  // Handle an incoming string.
  // If anything other than 0 returned, continue to take in strings
  int message(char str[]);

  // Takes a pointer to a printer for output
  void set_printer(Print* p);

 private:
  void switch_to_main_menu();
  void display_main_menu();
  void switch_to_menu();
  void display_menu();

  void add();
  void which_add();
  void poem();

  void continue_at(int stat);
  void resume();

  // Returns true when c is yes or no
  bool yn(char c);
  // Returns true when c is 'Y' or 'y'
  bool yes(char c);

  Print* p;

  // Status to resume to
  int resume_state;

  // Change to an existing login state
  int login();

  // List of users
  vector<const char*> users;

  // Current user
  char* user;

  // True when user has been greeted.
  bool started;

  // Create a new user and change to logged in state
  void new_user();

  // current state of the system
  // 0 means main menu
  int state;

  // If inside a complex function (like add), keeps track of internal status
  int substate;

  // Database representation
  Database db;

  // Current flashcard
  Card* card;
};

#endif
