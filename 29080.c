void GfxAxialShading::getColor(double t, GfxColor *color) {
  double out[gfxColorMaxComps];
  int i;

  for (i = 0; i < gfxColorMaxComps; ++i) {
    out[i] = 0;
  }
  for (i = 0; i < nFuncs; ++i) {
    funcs[i]->transform(&t, &out[i]);
  }
  for (i = 0; i < gfxColorMaxComps; ++i) {
    color->c[i] = dblToCol(out[i]);
  }
}
