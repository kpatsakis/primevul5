GfxFunctionShading::GfxFunctionShading(GfxFunctionShading *shading):
  GfxShading(shading)
{
  int i;

  x0 = shading->x0;
  y0 = shading->y0;
  x1 = shading->x1;
  y1 = shading->y1;
  for (i = 0; i < 6; ++i) {
    matrix[i] = shading->matrix[i];
  }
  nFuncs = shading->nFuncs;
  for (i = 0; i < nFuncs; ++i) {
    funcs[i] = shading->funcs[i]->copy();
  }
}
