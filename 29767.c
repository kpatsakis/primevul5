void FoFiType1C::eexecWrite(Type1CEexecBuf *eb, const char *s) {
  Guchar *p;
  Guchar x;

  for (p = (Guchar *)s; *p; ++p) {
    x = *p ^ (eb->r1 >> 8);
    eb->r1 = (x + eb->r1) * 52845 + 22719;
    if (eb->ascii) {
      (*eb->outputFunc)(eb->outputStream, &hexChars[x >> 4], 1);
      (*eb->outputFunc)(eb->outputStream, &hexChars[x & 0x0f], 1);
      eb->line += 2;
      if (eb->line == 64) {
	(*eb->outputFunc)(eb->outputStream, "\n", 1);
	eb->line = 0;
      }
    } else {
      (*eb->outputFunc)(eb->outputStream, (char *)&x, 1);
    }
  }
}
