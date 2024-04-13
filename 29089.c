void GfxICCBasedColorSpace::getDefaultColor(GfxColor *color) {
  int i;

  for (i = 0; i < nComps; ++i) {
    if (rangeMin[i] > 0) {
      color->c[i] = dblToCol(rangeMin[i]);
    } else if (rangeMax[i] < 0) {
      color->c[i] = dblToCol(rangeMax[i]);
    } else {
      color->c[i] = 0;
    }
  }
}
