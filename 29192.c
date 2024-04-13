GfxPatternColorSpace::~GfxPatternColorSpace() {
  if (under) {
    delete under;
  }
}
