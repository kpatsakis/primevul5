void ArthurOutputDev::startDoc(XRef *xrefA) {
  xref = xrefA;
  delete m_fontEngine;
  m_fontEngine = new SplashFontEngine(
#if HAVE_T1LIB_H
  globalParams->getEnableT1lib(),
#endif
#if HAVE_FREETYPE_FREETYPE_H || HAVE_FREETYPE_H
  globalParams->getEnableFreeType(),
#endif
  m_painter->testRenderHint(QPainter::TextAntialiasing));
}
