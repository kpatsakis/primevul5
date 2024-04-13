void SplashOutputDev::drawChar(GfxState *state, double x, double y,
			       double dx, double dy,
			       double originX, double originY,
			       CharCode code, int nBytes,
			       Unicode *u, int uLen) {
  SplashPath *path;
  int render;

  render = state->getRender();
  if (render == 3) {
    return;
  }

  if (needFontUpdate) {
    doUpdateFont(state);
  }
  if (!font) {
    return;
  }

  x -= originX;
  y -= originY;

  if (!(render & 1)) {
    if (!state->getFillColorSpace()->isNonMarking()) {
      splash->fillChar((SplashCoord)x, (SplashCoord)y, code, font);
    }
  }

  if ((render & 3) == 1 || (render & 3) == 2) {
    if (!state->getStrokeColorSpace()->isNonMarking()) {
      if ((path = font->getGlyphPath(code))) {
	path->offset((SplashCoord)x, (SplashCoord)y);
	splash->stroke(path);
	delete path;
      }
    }
  }

  if (render & 4) {
    if ((path = font->getGlyphPath(code))) {
      path->offset((SplashCoord)x, (SplashCoord)y);
      if (textClipPath) {
	textClipPath->append(path);
	delete path;
      } else {
	textClipPath = path;
      }
    }
  }
}
