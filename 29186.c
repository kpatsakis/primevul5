GfxImageColorMap::~GfxImageColorMap() {
  int i;

  delete colorSpace;
  for (i = 0; i < gfxColorMaxComps; ++i) {
    gfree(lookup[i]);
  }
  gfree(byte_lookup);
}
