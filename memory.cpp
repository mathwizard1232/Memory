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

int memory(int argc, char* argv[])
{
  initscr();
  timeout(0);
  curs_set(0);
  noecho();
  cbreak();
  Print out = Print();
  
  /*  ifstream file;
  int advance = 0;

  if (argc > 1) {
    file.open(argv[1],std::ios::in);
    if (argc > 2) {
      if (argv[2][0] == 'm') {
	ifstream mark("mark.txt");
	string current;
	while ((mark >> current) && (strcmp(current.c_str(),argv[1])!=0)) {
	}
	mark >> advance;					      
	mark.close();
	if (argv[2][1] == 'r') {
	  advance -= 500;
	  if (advance < 0)
	    advance = 0;
	}
      }
      else {
	char* endptr;
	int temp = strtol(argv[2], &endptr, 10);
	if (endptr && (*endptr == '\0'))
	  advance = temp;
      }
    }
  }
  else {
    ifstream mark("mark.txt");
    string current;
    mark >> current;
    string adv;
    mark >> adv;
    char* null;
    int advance = strtol(adv.c_str(),&null,10);
  }

  char str[2000];

  for (int i = 0; i < advance; i++)
    file >> str;

  int pos = advance;
  int lines = 3;
  int width = 40;
  int wait = 2;
  int current = 0;
  char temp[2000];
  temp[0] = '\0';

  int c;
  bool paused = false;
  while(file >> str) {
    c = getch();
    clrtoeol();
    if (c == 'q')
      break;

    if (c==' ') {
      paused = true;
      while (paused) {
        c = getch();
          if (c == ' ')
            paused = false;
        }
    }
    mvaddch(1,2,'c');
    if ((length(temp) + 1 + length(str) < width) || temp[0] == '\0') {
      if (temp[0] != '\0')
	add(" ", temp);
      add(str, temp);
    }
    else if (current < lines) {
      out.print(temp);
      temp[0] = '\0';
      add(str,temp);
      current++;
    }
    else {
      clear();
      out.refresh();
      refresh();
      sleep(wait);
      out.clear();
      out.print(temp);
      temp[0] = '\0';
      add(str,temp);
      current = 1;
      clear();
      out.refresh();
      refresh();
      }
    pos++;
  }

  ofstream mark("mark.txt");
  mark << argv[1] << " ";
  mark << pos;
  mark.close();
  */
  endwin(); 
  return 0;
}

#endif
