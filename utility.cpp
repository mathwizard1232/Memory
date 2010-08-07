#include "utility.h"
#include <string>
#include <fstream>

using std::istream;
using std::ofstream;
using std::string;

int length(const char* str)
{
  if (str[0] == '\0')
    return 0;
  else
    return 1 + length(str + 1);
}

// Copy with allocate
void copy_leak(const char* in, char*& out) {
  out = new char[length(in) + 1];
  copy(in,out);
}

int copy(const char* in, char* out) {
  int pos = 0;
  while (in[pos] != '\0') {
    out[pos] = in[pos];
    pos++;
  }
  out[pos] = '\0';
}

void add(const char* end, char* start) {
  int pos = 0;
  while (start[pos] != '\0') {
    pos++;
  }

  int pos2 = 0;
  while (end[pos2] != '\0') {
    start[pos++] = end[pos2++];
  }

  start[pos] = '\0';
}

void convert(char arr[], char* ptr) {
  int i = 0;
  while (arr[i] != '\0') {
    ptr[i] = arr[i];
    i++;
  }
  ptr[i] = arr[i];
}

bool charcmp(char a, char b) {
  int offset = 32;//'a'-'A';
  int ca = (int)a;
  int cb = (int)b;
  return (a==b) || (ca+offset==cb) || (cb+offset==ca);
}

void getline(istream& i, char*& out) {
  string in;
  getline(i, in);
  out = new char[in.size() + 1];
  copy(in.c_str(),out);
}

void log(const char* statement) {
  ofstream f("log.txt",ofstream::app);
  f << statement << std::endl;
}

void log() {
  ofstream f("log.txt",ofstream::trunc);
}

void log(int i) {
  char c[20];
  sprintf(c,"%i",i);
  log(c);
}

void log(const string str) {
  log(str.c_str());
}

int find(char* src, const char c, int index) {
  if (src[0] == c)
    return index;
  if (src[0] == '\0')
    return -1;
  return find(++src,c,++index);
}

int find(char* src, const char c) {
  find(src,c,0);
}

void split(char*& src, char point, char*& top) {
  int index = find(src,point);
  top = new char[index+1];
  for (int i = 0; i < index; i++) {
    top[i] = src[0];
    src++;
  }
  src++;
  top[index] = '\0';
}
