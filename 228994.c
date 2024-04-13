CairoFont::~CairoFont() {
  FT_Done_Face (face);
  gfree(codeToGID);
}