#ifndef PRINT
#define PRINT

#include <fstream>
#include <vector>
#include <string>
using std::ofstream;
using std::vector;

class Print {
 public:
  Print();
  ~Print();

  void newline(); // Prints a newline

  // Add the given line to the buffer to output.
  // Note: by default Print does (not (?)) add a '\n' between each output // This needs testing / examination.
  void print(const char str[], bool no_refresh = false);
  //  void print(const char str[]);
  void printf(const char format[],const char arg[], bool no_refresh = false);
  // Clear the screen
  void cls(bool refresh = false);

  // Add the following sequential elements preceeded by #.
  void list(const vector<const char*> l);
  void list(const vector<std::string> l);

  void redraw(int s = 0); // which screen to print

  bool print_more(); // returns false if more printed, else true for done
 private:
  int screen;
  int lines;
  int allocated;
  char** output;
  
  //ofstream* log;

  void resize();

};

#endif
