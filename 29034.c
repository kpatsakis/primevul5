GfxRadialShading::GfxRadialShading(GfxRadialShading *shading):
  GfxShading(shading)
{
  int i;

  x0 = shading->x0;
  y0 = shading->y0;
  r0 = shading->r0;
  x1 = shading->x1;
  y1 = shading->y1;
  r1 = shading->r1;
  t0 = shading->t0;
  y1 = shading->t1;
  nFuncs = shading->nFuncs;
  for (i = 0; i < nFuncs; ++i) {
    funcs[i] = shading->funcs[i]->copy();
  }
  extend0 = shading->extend0;
  extend1 = shading->extend1;
}
