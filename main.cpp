#include <fstream>
#include <curses.h>
#include "print.h"
#include "utility.h"
#include <stdlib.h>
#include <cstring>
#include <string>
#include <iostream>

using namespace std;

// General higher-level abstraction of the memory system
#include "memory.h"
Memory m;

int main(int argc, char* argv[])
{
  log();
  Print out = Print();

  m.set_printer(&out);
  char c;
  char str[80];

  bool mode = false; // If true, take in string. If false, take in char.
  while (true) {
    if (mode) {
      echo();
      getstr(str);
      noecho();
      mode = mode ^ m.message(str); // If message returns != 0, then make mode false
    }
    else {
      c = getch();
      clrtoeol();
    
      if (c == 'q')
        break;
      
      if ((c != ' ') || (out.print_more())) // print_more() returns 1 if no more
        mode = mode ^ m.message(c); // If message returns != 0, then make mode true
    }
  }

  endwin(); 
  return 0;
}
