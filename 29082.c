void GfxImageColorMap::getColor(Guchar *x, GfxColor *color) {
  int maxPixel, i;

  maxPixel = (1 << bits) - 1;
  for (i = 0; i < nComps; ++i) {
    color->c[i] = dblToCol(decodeLow[i] + (x[i] * decodeRange[i]) / maxPixel);
  }
}
