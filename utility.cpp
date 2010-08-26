#include "utility.h"
#include <string>
#include <fstream>

using std::istream;
using std::ofstream;
using std::string;

/*int length(const char* str)
{
  if (str[0] == '\0')
    return 0;
  else
    return 1 + length(str + 1);
    }*/

int length(const char* str) {
  int l = 0;
  while (str[l] != '\0') {
    l++;
  }
  return l;
}

// Copy with allocate
void copy_leak(const char* in, char*& out) {
  length(in);
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

int min(int a, int b) {
  if (a < b)
    return a;
  return b;
}

/* Thanks and glory be to http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html */
// Modified for multicharacter delimiter
vector<string> split(const string& str, const string& delimiter = " ")
{
  vector<string> tokens;
  int current = 0;
  // Find first split point.
  string::size_type end = str.find(delimiter, 0);
  // Find end of split point (start of next block).
  //string::size_type next    = lastPos + delimiter.length();

  while (string::npos != end) {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(current, end-current));
    // Skip the delimiter.
    current = end + delimiter.length(); 
    // Find next "delimiter"
    end = str.find(delimiter, current);
  }

  if (current < str.size()-1) {
    tokens.push_back(str.substr(current));
  }
  return tokens;
}
#include <sstream>
using std::stringstream;
string i_str(int i) {
  stringstream ss;
  ss << i;
  return ss.str();
}
