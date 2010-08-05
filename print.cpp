#include "print.h"
#include "utility.h"
#include <curses.h>
#include <fstream>
#include <string>
#include <string.h>

using std::endl;
using std::string;

Print::Print()
  :lines(0), allocated(0), output()//, log("printlog.txt")
{
  //this->log = new ofstream("printlog.txt");
  //*(this->log) << "Text";
  this->resize();

  //  initscr();
  //  timeout(0); // Apparently means continuously reading input.
  //  ^ useful if we need to react after a given time whether or not user replies.
  //  curs_set(0);
  //  noecho();
  initscr(); // Start curses
  cbreak(); // Don't wait until newline for input
  noecho(); // If going to display, will do it manually
  curs_set(0); // Don't display the cursor
}

Print::~Print()
{
  //this->log->close();
  
  //ofstream file("closefile.txt");
}

void Print::printf(const char form[], const char arg[]) {
  char out[length(form)+length(arg)];
  sprintf(out,form,arg);
  print(out);
}

void Print::print(const char in[])
{
  // Note: right now I leak memory here. Should change before program becomes mission-critical.
  // If you're reading this comment and I didn't, I'm sorry. On the other hand, clearly the code was worth
  // maintaining, so you're welcome.
  char* str = new char[length(in)+1];
  copy(in,str);
  if (find(str,'\n') != -1) {

  }

  if (this->lines < this->allocated)
    this->output[this->lines++] = str;
  else {
    this->resize();
    if (this->lines < this->allocated)
      this->output[this->lines++] = str;
    else {
      // Unable to increase allocation. This is not normal. Crash.
      int a = 0;
      1/a;
    }
  }
  
  this->redraw();
}

/*void Print::print(const char in[]) {
  print(&in);
  }*/

void Print::redraw()
{
  clear();
  int maxwidth = 0;

  for (int i = 0; i < this->lines; i++) {
    if (length(this->output[i]) > maxwidth)
      maxwidth = length(this->output[i]);
  }

  int padding = 0;
  if (maxwidth < COLS) {
    padding = (COLS - maxwidth) / 2;
  }

  int skip = 0;
  if (this->lines < LINES) {
    skip = (LINES - this->lines) / 2;
  }

  for (int i = 0; i < this->lines; i++) {
    int pos = 0;
    while (this->output[i][pos] != '\0') {
      mvaddch(skip + i + 1, padding + pos + 1, this->output[i][pos]);
      pos++;
    }
  }
  refresh();
}

// Get rid of internal storage of characters
void Print::cls()
{
  // Probably leaking memory here.
  this->lines = 0;

  this->redraw();
}

void Print::resize()
{
  int newalloc = this->allocated * 2 + 10;
  char** newptr = new char*[newalloc];
  
  for (int i = 0; i < this->allocated; i++) {
    newptr[i] = this->output[i];
  }

  if (this->output != 0) {
    delete(this->output);
  }

  this->output = newptr;
  this->allocated = newalloc;
}

void Print::list(vector<const char*> l) {
  char out[200];
  for (int i = 0; i < l.size(); i++) {
    sprintf(out,"%i. ",i+1);
    strcat(out,l[i]);
    print(out);
  }
}
