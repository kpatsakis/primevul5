void CairoOutputDev::clearSoftMask(GfxState * /*state*/) {
  if (mask)
    cairo_pattern_destroy(mask);
  mask = NULL;
}
