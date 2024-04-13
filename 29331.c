GBool SplashFTFont::getGlyph(int c, int xFrac, int yFrac,
			     SplashGlyphBitmap *bitmap, int x0, int y0, SplashClip *clip, SplashClipResult *clipRes) {
  return SplashFont::getGlyph(c, xFrac, 0, bitmap, x0, y0, clip, clipRes);
}
