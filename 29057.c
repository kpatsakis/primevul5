GfxColorSpace *GfxSeparationColorSpace::copy() {
  return new GfxSeparationColorSpace(name->copy(), alt->copy(), func->copy());
}
