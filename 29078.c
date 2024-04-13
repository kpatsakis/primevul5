void GfxImageColorMap::getCMYK(Guchar *x, GfxCMYK *cmyk) {
  GfxColor color;
  int i;

  if (colorSpace2) {
    for (i = 0; i < nComps2; ++i) {
      color.c[i] = lookup[i][x[0]];
    }
    colorSpace2->getCMYK(&color, cmyk);
  } else {
    for (i = 0; i < nComps; ++i) {
      color.c[i] = lookup[i][x[i]];
    }
    colorSpace->getCMYK(&color, cmyk);
  }
}
