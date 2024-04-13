GfxShading::GfxShading(GfxShading *shading) {
  int i;

  type = shading->type;
  colorSpace = shading->colorSpace->copy();
  for (i = 0; i < gfxColorMaxComps; ++i) {
    background.c[i] = shading->background.c[i];
  }
  hasBackground = shading->hasBackground;
  xMin = shading->xMin;
  yMin = shading->yMin;
  xMax = shading->xMax;
  yMax = shading->yMax;
  hasBBox = shading->hasBBox;
}
