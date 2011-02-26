#include <curses.h>
#include "print.h"
using namespace std;

// General higher-level abstraction of the memory system
#include "memory.h"
Memory m;

const int LINE_BUFFER = 1000;
int main(int argc, char* argv[])
{
  log(); // Touch the log file.
  Print out = Print(); // Initialize the output object.

  m.set_printer(&out); // Tell memory where to send output.
  char c; // space for single character input
  char str[LINE_BUFFER]; // space for single line input

  bool mode = false; // If true, take in string. If false, take in char.
  while (true) { // Exit with 'q' in single character input modes.
    if (mode) { // String input
      echo();
      curs_set(1);
      getstr(str);
      noecho();
      curs_set(0);
      mode = mode ^ m.message(str); // If message returns != 0, then make mode false
    }
    else { // Character input
      c = getch();
      clrtoeol();
    
      if (c == 'q') // Quit program.
        break;
      
      if ((c != ' ') || (out.print_more())) // print_more() returns 1 if no more
        mode = mode ^ m.message(c); // If message returns != 0, then make mode true
    }
  }

  endwin(); 
  return 0;
}
