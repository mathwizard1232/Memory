#ifndef MEMORY_OBJ
#define MEMORY_OBJ

#include "print.h"
#include "database.h"

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
  Print* p;

  // Change to an existing login state
  void login();

  // Create a new user and change to logged in state
  void new_user();

  // current state of the system
  // 0 means main menu
  int state;

  // Database representation
  Database db;
};

#endif
