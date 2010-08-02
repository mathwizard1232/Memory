#include "memory.h"
#include "utility.h"

Memory::Memory()
  :state(0)
{
}

void Memory::set_printer(Print* in) {
  p = in;
}

int Memory::message(char c) {
  int a = (int)c;

  char* b = new char[4];
  sprintf(b,"%i",a);
  // Handle the character if logical
  if (c != '\0' && a != -1) {
    if (state == 0) {
      // main menu
      if (charcmp(c,'l')) {
        login();
      }
      else if (charcmp(c,'n')) {
        new_user();
        //echo();
        return 1; // Switch to taking in a string. (name)
      }
      else {
        p->print(&c);
      }
    }
  }
  return 0;
}

int Memory::message(char str[]) {
  char* out = new char[200];
  db.add_user(str);
  sprintf(out,"Your name is: %s",str);
  p->print(out);
  return 1;
}

void Memory::login() {
}

void Memory::new_user() {
  p->cls();
  p->print("What would you like to be called?");
}
