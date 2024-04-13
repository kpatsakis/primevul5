void JBIG2Stream::close() {
  if (pageBitmap) {
    delete pageBitmap;
    pageBitmap = NULL;
  }
  if (segments) {
    deleteGooList(segments, JBIG2Segment);
    segments = NULL;
  }
  if (globalSegments) {
    deleteGooList(globalSegments, JBIG2Segment);
    globalSegments = NULL;
  }
  dataPtr = dataEnd = NULL;
  FilterStream::close();
}
