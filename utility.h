int length(const char* str);
int copy(const char* in, char* out);
void copy_leak(const char* in, char*& out);
void add(const char* end, char* start);
void convert(char arr[], char* ptr);
// Case-insensitive character comparison
bool charcmp(char a, char b);
#include <fstream>
void getline(std::istream& i, char*& out);
void log(const char*);
void log(int);
void log();
#include <string>
void log(const std::string);
int find(char* src, const char c);
void split(char*& src, char point, char*& top);
int min(int, int);
int max(int, int);
#include <vector>
using std::vector;
vector<std::string> split(const std::string& str, const std::string& delimiter);
std::string i_str(int i);
const char* i_cstr(int i);
