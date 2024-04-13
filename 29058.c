GfxColorSpace *GfxPatternColorSpace::copy() {
  return new GfxPatternColorSpace(under ? under->copy() :
				          (GfxColorSpace *)NULL);
}
