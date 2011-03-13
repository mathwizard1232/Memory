#ifndef MEMORY_OBJ
#define MEMORY_OBJ

#include "print.h"
#include "database.h"
#include "card.h"

// Need to be loaded from the database during initialization
// We trigger that during Memory construction.
#include "categories.h"

class Memory {
 public:
  Memory();
  ~Memory();

  // Handle an incoming character.
  // If anything other than 0 returned, switch to line input
  int message(char c);

  // Handle an incoming string.
  // If anything other than 0 returned, continue to take in strings
  int message(const char str[]);

  // Takes a pointer to a printer for output
  void set_printer(Print* p);

 private:
  int (Memory::*cmessagep)(char); // cmessagep points to the character message handler for the current state.
  int (Memory::*strmessagep)(const char[]); // same for strings
  int nullcmessage(char); // These are the initial values for above. They do nothing but can be called without seg fault.
  int nullstrmessage(const char[]);

  int create_user_str(const char[]); // This is one of the above type of message handlers.
  int which_add_c(char); // Select which method of input to use.
  int review_c(char); // Handles the review process using substates.
  int menu_c(char); // This is the main logged-in menu
  int main_menu_c(char) ; // Login screen shown when starting
  int manage_c(char); // Managing knowledge

  void categories(); // These functions manage the interface for categories.
  int categories_c(char);
  int categories_str(const char[]);
  void child();
  void suffix();

  int add_c(char);
  int add_str(const char[]);
  int poem_c(char);
  int poem_str(const char[]);

  int press_key_c(char);
  int login_c(char);
  int login_str(const char[]);

  void switch_to_main_menu();
  void display_main_menu();
  void switch_to_menu();
  void display_menu();

  void add();
  void which_add();
  void poem();

  void review();

  void manage();
  void dump();

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
  Category* cat; // Current category


  // True when user has been greeted.
  bool started;

  // Create a new user and change to logged in state
  void new_user();

  // If inside a complex function (like add), keeps track of internal status
  int substate;

  // Database representation
  Database db;

  // Current flashcard
  Card* card;
};

#endif
