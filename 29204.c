JBIG2SymbolDict::JBIG2SymbolDict(Guint segNumA, Guint sizeA):
  JBIG2Segment(segNumA)
{
  size = sizeA;
  bitmaps = (JBIG2Bitmap **)gmallocn(size, sizeof(JBIG2Bitmap *));
  genericRegionStats = NULL;
  refinementRegionStats = NULL;
}
