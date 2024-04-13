void GfxImageColorMap::getGray(Guchar *x, GfxGray *gray) {
  GfxColor color;
  int i;

  if (colorSpace2) {
    for (i = 0; i < nComps2; ++i) {
      color.c[i] = lookup[i][x[0]];
    }
    colorSpace2->getGray(&color, gray);
  } else {
    for (i = 0; i < nComps; ++i) {
      color.c[i] = lookup[i][x[i]];
    }
    colorSpace->getGray(&color, gray);
  }
}
