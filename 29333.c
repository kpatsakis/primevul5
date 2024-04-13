SplashPath *SplashFTFont::getGlyphPath(int c) {
  static FT_Outline_Funcs outlineFuncs = {
#if FREETYPE_MINOR <= 1
    (int (*)(FT_Vector *, void *))&glyphPathMoveTo,
    (int (*)(FT_Vector *, void *))&glyphPathLineTo,
    (int (*)(FT_Vector *, FT_Vector *, void *))&glyphPathConicTo,
    (int (*)(FT_Vector *, FT_Vector *, FT_Vector *, void *))&glyphPathCubicTo,
#else
    &glyphPathMoveTo,
    &glyphPathLineTo,
    &glyphPathConicTo,
    &glyphPathCubicTo,
#endif
    0, 0
  };
  SplashFTFontFile *ff;
  SplashFTFontPath path;
  FT_GlyphSlot slot;
  FT_UInt gid;
  FT_Glyph glyph;

  ff = (SplashFTFontFile *)fontFile;
  ff->face->size = sizeObj;
  FT_Set_Transform(ff->face, &textMatrix, NULL);
  slot = ff->face->glyph;
  if (ff->codeToGID && c < ff->codeToGIDLen) {
    gid = ff->codeToGID[c];
  } else {
    gid = (FT_UInt)c;
  }
  if (ff->trueType && gid == 0) {
    return NULL;
  }
  if (FT_Load_Glyph(ff->face, gid, FT_LOAD_NO_BITMAP)) {
    return NULL;
  }
  if (FT_Get_Glyph(slot, &glyph)) {
    return NULL;
  }
  path.path = new SplashPath();
  path.textScale = textScale;
  path.needClose = gFalse;
  FT_Outline_Decompose(&((FT_OutlineGlyph)glyph)->outline,
		       &outlineFuncs, &path);
  if (path.needClose) {
    path.path->close();
  }
  FT_Done_Glyph(glyph);
  return path.path;
}
