void FoFiType1C::eexecWriteCharstring(Type1CEexecBuf *eb,
				      Guchar *s, int n) {
  Guchar x;
  int i;

  for (i = 0; i < n; ++i) {
    x = s[i] ^ (eb->r1 >> 8);
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
