#ifndef PRINT
#define PRINT

#include <fstream>
#include <vector>
using std::ofstream;
using std::vector;

class Print {
 public:
  Print();
  ~Print();

  // Add the given line to the buffer to output.
  // Note: by default Print adds a '\n' between each output
  void print(const char str[]);
  //  void print(const char str[]);
  void printf(const char format[],const char arg[]);
  // Clear the screen
  void cls();

  // Add the following sequential elements preceeded by #.
  void list(const vector<const char*> l);

 private:
  int lines;
  int allocated;
  char** output;
  
  //ofstream* log;

  void resize();

  void redraw();

};

#endif
