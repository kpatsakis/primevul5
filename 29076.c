void GfxSeparationColorSpace::getCMYK(GfxColor *color, GfxCMYK *cmyk) {
  double x;
  double c[gfxColorMaxComps];
  GfxColor color2;
  int i;

  x = colToDbl(color->c[0]);
  func->transform(&x, c);
  for (i = 0; i < alt->getNComps(); ++i) {
    color2.c[i] = dblToCol(c[i]);
  }
  alt->getCMYK(&color2, cmyk);
}
