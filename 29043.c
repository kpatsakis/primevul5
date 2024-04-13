GfxTilingPattern::GfxTilingPattern(int paintTypeA, int tilingTypeA,
				   double *bboxA, double xStepA, double yStepA,
				   Object *resDictA, double *matrixA,
				   Object *contentStreamA):
  GfxPattern(1)
{
  int i;

  paintType = paintTypeA;
  tilingType = tilingTypeA;
  for (i = 0; i < 4; ++i) {
    bbox[i] = bboxA[i];
  }
  xStep = xStepA;
  yStep = yStepA;
  resDictA->copy(&resDict);
  for (i = 0; i < 6; ++i) {
    matrix[i] = matrixA[i];
  }
  contentStreamA->copy(&contentStream);
}
