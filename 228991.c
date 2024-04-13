CairoFontEngine::getFont(GfxFont *gfxFont, XRef *xref) {
  int i, j;
  Ref ref;
  CairoFont *font;
  GfxFontType fontType;
  
  fontType = gfxFont->getType();
  if (fontType == fontType3) {
    /* Need to figure this out later */
    //    return NULL;
  }

  ref = *gfxFont->getID();

  for (i = 0; i < cairoFontCacheSize; ++i) {
    font = fontCache[i];
    if (font && font->matches(ref)) {
      for (j = i; j > 0; --j) {
	fontCache[j] = fontCache[j-1];
      }
      fontCache[0] = font;
      return font;
    }
  }
  
  font = CairoFont::create (gfxFont, xref, lib, useCIDs);
  //XXX: if font is null should we still insert it into the cache?
  if (fontCache[cairoFontCacheSize - 1]) {
    delete fontCache[cairoFontCacheSize - 1];
  }
  for (j = cairoFontCacheSize - 1; j > 0; --j) {
    fontCache[j] = fontCache[j-1];
  }
  fontCache[0] = font;
  return font;
}