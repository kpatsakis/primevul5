void FoFiType1C::writePSString(char *s, FoFiOutputFunc outputFunc,
			       void *outputStream) {
  char buf[80];
  char *p;
  int i, c;

  i = 0;
  buf[i++] = '(';
  for (p = s; *p; ++p) {
    c = *p & 0xff;
    if (c == '(' || c == ')' || c == '\\') {
      buf[i++] = '\\';
      buf[i++] = c;
    } else if (c < 0x20 || c >= 0x80) {
      buf[i++] = '\\';
      buf[i++] = '0' + ((c >> 6) & 7);
      buf[i++] = '0' + ((c >> 3) & 7);
      buf[i++] = '0' + (c & 7);
    } else {
      buf[i++] = c;
    }
    if (i >= 64) {
      buf[i++] = '\\';
      buf[i++] = '\n';
      (*outputFunc)(outputStream, buf, i);
      i = 0;
    }
  }
  buf[i++] = ')';
  (*outputFunc)(outputStream, buf, i);
}
