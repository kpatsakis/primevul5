JBIG2Stream::JBIG2Stream(Stream *strA, Object *globalsStreamA):
  FilterStream(strA)
{
  pageBitmap = NULL;

  arithDecoder = new JArithmeticDecoder();
  genericRegionStats = new JArithmeticDecoderStats(1 << 1);
  refinementRegionStats = new JArithmeticDecoderStats(1 << 1);
  iadhStats = new JArithmeticDecoderStats(1 << 9);
  iadwStats = new JArithmeticDecoderStats(1 << 9);
  iaexStats = new JArithmeticDecoderStats(1 << 9);
  iaaiStats = new JArithmeticDecoderStats(1 << 9);
  iadtStats = new JArithmeticDecoderStats(1 << 9);
  iaitStats = new JArithmeticDecoderStats(1 << 9);
  iafsStats = new JArithmeticDecoderStats(1 << 9);
  iadsStats = new JArithmeticDecoderStats(1 << 9);
  iardxStats = new JArithmeticDecoderStats(1 << 9);
  iardyStats = new JArithmeticDecoderStats(1 << 9);
  iardwStats = new JArithmeticDecoderStats(1 << 9);
  iardhStats = new JArithmeticDecoderStats(1 << 9);
  iariStats = new JArithmeticDecoderStats(1 << 9);
  iaidStats = new JArithmeticDecoderStats(1 << 1);
  huffDecoder = new JBIG2HuffmanDecoder();
  mmrDecoder = new JBIG2MMRDecoder();

  globalsStreamA->copy(&globalsStream);
  segments = globalSegments = NULL;
  curStr = NULL;
  dataPtr = dataEnd = NULL;
}
