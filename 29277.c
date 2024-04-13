void Splash::fillGlyph(SplashCoord x, SplashCoord y,
			      SplashGlyphBitmap *glyph) {
  SplashCoord xt, yt;
  int x0, y0;

  transform(state->matrix, x, y, &xt, &yt);
  x0 = splashFloor(xt);
  y0 = splashFloor(yt);
  SplashClipResult clipRes = state->clip->testRect(x0 - glyph->x,
                             y0 - glyph->y,
                             x0 - glyph->x + glyph->w - 1,
                             y0 - glyph->y + glyph->h - 1);
  if (clipRes != splashClipAllOutside) {
    fillGlyph2(x0, y0, glyph, clipRes == splashClipAllInside);
  }
  opClipRes = clipRes;
}
