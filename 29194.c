GfxShading::~GfxShading() {
  if (colorSpace) {
    delete colorSpace;
  }
}
