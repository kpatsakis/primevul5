void GfxFunctionShading::getColor(double x, double y, GfxColor *color) {
  double in[2], out[gfxColorMaxComps];
  int i;

  for (i = 0; i < gfxColorMaxComps; ++i) {
    out[i] = 0;
  }
  in[0] = x;
  in[1] = y;
  for (i = 0; i < nFuncs; ++i) {
    funcs[i]->transform(in, &out[i]);
  }
  for (i = 0; i < gfxColorMaxComps; ++i) {
    color->c[i] = dblToCol(out[i]);
  }
}
