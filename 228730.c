static int PeekMatch(FILE *stream, const char * target) {
  // This returns 1 if target matches the next characters in the stream.
  int pos1 = 0;
  int lastread = getc(stream);
  while (target[pos1] != '\0' && lastread != EOF && lastread == target[pos1]) {
    pos1 ++; lastread = getc(stream);
  }
  
  int rewind_amount = pos1 + ((lastread == EOF) ? 0 : 1);
  fseek(stream, -rewind_amount, SEEK_CUR);
  return (target[pos1] == '\0');
}