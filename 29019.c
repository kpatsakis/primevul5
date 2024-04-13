GfxFunctionShading::GfxFunctionShading(double x0A, double y0A,
				       double x1A, double y1A,
				       double *matrixA,
				       Function **funcsA, int nFuncsA):
  GfxShading(1)
{
  int i;

  x0 = x0A;
  y0 = y0A;
  x1 = x1A;
  y1 = y1A;
  for (i = 0; i < 6; ++i) {
    matrix[i] = matrixA[i];
  }
  nFuncs = nFuncsA;
  for (i = 0; i < nFuncs; ++i) {
    funcs[i] = funcsA[i];
  }
}
