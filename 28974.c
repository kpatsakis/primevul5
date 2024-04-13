void CairoOutputDev::endType3Char(GfxState *state) {
  cairo_restore (cairo);
  if (cairo_shape) {
    cairo_restore (cairo_shape);
  }
}
