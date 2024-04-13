int *FoFiType1C::getCIDToGIDMap(int *nCIDs) {
  int *map;
  int n, i;

  if (topDict.firstOp != 0x0c1e) {
    *nCIDs = 0;
    return NULL;
  }

  n = 0;
  for (i = 0; i < nGlyphs && i < charsetLength; ++i) {
    if (charset[i] > n) {
      n = charset[i];
    }
  }
  ++n;
  map = (int *)gmallocn(n, sizeof(int));
  memset(map, 0, n * sizeof(int));
  for (i = 0; i < nGlyphs; ++i) {
    map[charset[i]] = i;
  }
  *nCIDs = n;
  return map;
}
