GfxGouraudTriangleShading::GfxGouraudTriangleShading(
			       GfxGouraudTriangleShading *shading):
  GfxShading(shading)
{
  int i;

  nVertices = shading->nVertices;
  vertices = (GfxGouraudVertex *)gmallocn(nVertices, sizeof(GfxGouraudVertex));
  memcpy(vertices, shading->vertices, nVertices * sizeof(GfxGouraudVertex));
  nTriangles = shading->nTriangles;
  triangles = (int (*)[3])gmallocn(nTriangles * 3, sizeof(int));
  memcpy(triangles, shading->triangles, nTriangles * 3 * sizeof(int));
  nFuncs = shading->nFuncs;
  for (i = 0; i < nFuncs; ++i) {
    funcs[i] = shading->funcs[i]->copy();
  }
}
