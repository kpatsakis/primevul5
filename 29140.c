GfxColor *GfxIndexedColorSpace::mapColorToBase(GfxColor *color,
					       GfxColor *baseColor) {
  Guchar *p;
  double low[gfxColorMaxComps], range[gfxColorMaxComps];
  int n, i;

  n = base->getNComps();
  base->getDefaultRanges(low, range, indexHigh);
  p = &lookup[(int)(colToDbl(color->c[0]) + 0.5) * n];
  for (i = 0; i < n; ++i) {
    baseColor->c[i] = dblToCol(low[i] + (p[i] / 255.0) * range[i]);
  }
  return baseColor;
}
