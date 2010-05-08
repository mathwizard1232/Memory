#ifndef PRINT
#define PRINT

#include <fstream>
using std::ofstream;

class Print {
 public:
  Print();
  ~Print();

  // Add the given line to the buffer to output.
  // Note: by default Print adds a '\n' between each output
  void print(const char* str);

  // Clear the screen
  void clear();

  // Redraw the screen
  void refresh();
 private:
  int lines;
  int allocated;
  char** output;
  
  ofstream* log;

  void resize();
  //  void refresh();
};

#endif
