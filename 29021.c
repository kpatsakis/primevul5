GfxGouraudTriangleShading::GfxGouraudTriangleShading(
			       int typeA,
			       GfxGouraudVertex *verticesA, int nVerticesA,
			       int (*trianglesA)[3], int nTrianglesA,
			       Function **funcsA, int nFuncsA):
  GfxShading(typeA)
{
  int i;

  vertices = verticesA;
  nVertices = nVerticesA;
  triangles = trianglesA;
  nTriangles = nTrianglesA;
  nFuncs = nFuncsA;
  for (i = 0; i < nFuncs; ++i) {
    funcs[i] = funcsA[i];
  }
}
