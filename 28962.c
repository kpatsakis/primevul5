void CairoOutputDev::drawChar(GfxState *state, double x, double y,
			      double dx, double dy,
			      double originX, double originY,
			      CharCode code, int nBytes, Unicode *u, int uLen)
{
  if (currentFont) {
    glyphs[glyphCount].index = currentFont->getGlyph (code, u, uLen);
    glyphs[glyphCount].x = x - originX;
    glyphs[glyphCount].y = y - originY;
    glyphCount++;
  }

  if (!text)
    return;
  actualText->addChar (state, x, y, dx, dy, code, nBytes, u, uLen);
}
