SplashError Splash::fillChar(SplashCoord x, SplashCoord y,
			     int c, SplashFont *font) {
  SplashGlyphBitmap glyph;
  SplashCoord xt, yt;
  int x0, y0, xFrac, yFrac;
  SplashClipResult clipRes;

  if (debugMode) {
    printf("fillChar: x=%.2f y=%.2f c=%3d=0x%02x='%c'\n",
	   (double)x, (double)y, c, c, c);
  }
  transform(state->matrix, x, y, &xt, &yt);
  x0 = splashFloor(xt);
  xFrac = splashFloor((xt - x0) * splashFontFraction);
  y0 = splashFloor(yt);
  yFrac = splashFloor((yt - y0) * splashFontFraction);
  if (!font->getGlyph(c, xFrac, yFrac, &glyph, x0, y0, state->clip, &clipRes)) {
    return splashErrNoGlyph;
  }
  if (clipRes != splashClipAllOutside) {
    fillGlyph2(x0, y0, &glyph, clipRes == splashClipAllInside);
  }
  opClipRes = clipRes;
  if (glyph.freeData) {
    gfree(glyph.data);
  }
  return splashOk;
}
