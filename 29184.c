GfxGouraudTriangleShading::~GfxGouraudTriangleShading() {
  int i;

  gfree(vertices);
  gfree(triangles);
  for (i = 0; i < nFuncs; ++i) {
    delete funcs[i];
  }
}
