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

void Print::newline() {
  print("\n\n");
}

void Print::printf(const char form[], const char arg[], bool no_refresh) {
  char out[length(form)+length(arg)];
  sprintf(out,form,arg);
  print(out,no_refresh);
}

void Print::print(const char in[], bool no_refresh)
{
  // Note: right now I leak memory here. Should change before program becomes mission-critical.
  // If you're reading this comment and I didn't, I'm sorry. On the other hand, clearly the code was worth
  // maintaining, so you're welcome.
  char* str = new char[length(in)+1];
  copy(in,str);
  if (find(str,'\n') != -1) {
    char* top;
    split(str,'\n',top);
    print(top, no_refresh);
    print(str, no_refresh);
    return;
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
  
  if (!no_refresh) 
    this->redraw();
}

/*void Print::print(const char in[]) {
  print(&in);
  }*/

void Print::redraw(int s) // screen to print
{
  clear();
  screen = s;
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

  int offset = screen*(LINES-2);
  for (int i = 0; i < min(this->lines-offset,LINES-2); i++) {
    int pos = 0;
    while (this->output[i+offset][pos] != '\0') {
      mvaddch(skip + i + 1, padding + pos + 1, this->output[i+offset][pos]);
      pos++;
    }
  }
  // if more screens, prompt
  if (this->lines > offset + LINES - 2) {
    const char a[] = "Press space to see more";
    int pos = 0;
    while (a[pos] != '\0') {
      mvaddch(LINES-1, padding+pos+1, a[pos]);
      pos++;
    }
  }
  refresh();
}

// return true when this is called on completed output
bool Print::print_more() {
  if (this->lines > (screen + 1)*(LINES-2)) {
    redraw(++screen);
    return false;
  } else {
    return true;
  }
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

void Print::list(vector<string> l) {
  vector<const char*> convert;
  for (int i = 0; i < l.size(); i++) {
    convert.push_back(l[i].c_str());
  }
  list(convert);
}
