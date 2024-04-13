GfxPattern *GfxShadingPattern::copy() {
  return new GfxShadingPattern(shading->copy(), matrix);
}
