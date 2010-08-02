#ifndef MEMORY_CPP
#define MEMORY_CPP

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

int memory(int argc, char* argv[])
{
  Print out = Print();
  char a[] = "Welcome to memory.";
  out.print(a);
  out.print("(L)ogin");
  out.print("Create a (n)ew user");

  m.set_printer(&out);
  char c;
  char str[80];

  bool mode = false; // If true, take in string. If false, take in char.
  while (true) {
    if (mode) {
      echo();
      getstr(str);
      mode = mode ^ m.message(str); // If message returns != 0, then make mode false
    }
    else {
      c = getch();
      clrtoeol();
    
      if (c == 'q')
        break;
      else
        mode = mode ^ m.message(c); // If message returns != 0, then make mode true
    }
  }

  endwin(); 
  return 0;
}

#endif
