int length(const char* str)
{
  if (str[0] == '\0')
    return 0;
  else
    return 1 + length(str + 1);
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
