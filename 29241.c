void JBIG2Stream::readEndOfStripeSeg(Guint length) {
  Guint i;

  for (i = 0; i < length; ++i) {
    curStr->getChar();
  }
}
