GfxShadingPattern::GfxShadingPattern(GfxShading *shadingA, double *matrixA):
  GfxPattern(2)
{
  int i;

  shading = shadingA;
  for (i = 0; i < 6; ++i) {
    matrix[i] = matrixA[i];
  }
}
