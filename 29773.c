GooString *FoFiType1C::getGlyphName(int gid) {
  char buf[256];
  GBool ok;

  ok = gTrue;
  if (gid < 0 || gid >= charsetLength)
    return NULL;
  getString(charset[gid], buf, &ok);
  if (!ok) {
    return NULL;
  }
  return new GooString(buf);
}
