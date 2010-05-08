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
