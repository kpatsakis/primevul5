double SplashFTFont::getGlyphAdvance(int c)
{
  SplashFTFontFile *ff;
  FT_Vector offset;
  FT_UInt gid;
  FT_Matrix identityMatrix;

  ff = (SplashFTFontFile *)fontFile;

  identityMatrix.xx = 65536; // 1 in 16.16 format
  identityMatrix.xy = 0;
  identityMatrix.yx = 0;
  identityMatrix.yy = 65536; // 1 in 16.16 format

  offset.x = 0;
  offset.y = 0;

  ff->face->size = sizeObj;
  FT_Set_Transform(ff->face, &identityMatrix, &offset);

  if (ff->codeToGID && c < ff->codeToGIDLen) {
    gid = (FT_UInt)ff->codeToGID[c];
  } else {
    gid = (FT_UInt)c;
  }
  if (ff->trueType && gid == 0) {
    return -1;
  }

#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER
  if (FT_Load_Glyph(ff->face, gid,
		    aa ? FT_LOAD_NO_BITMAP : FT_LOAD_DEFAULT)) {
    return -1;
  }
#else
  if (FT_Load_Glyph(ff->face, gid,
		    aa ? FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP
                       : FT_LOAD_DEFAULT)) {
    return -1;
  }
#endif

  return ff->face->glyph->metrics.horiAdvance / 64.0 / size;
}
