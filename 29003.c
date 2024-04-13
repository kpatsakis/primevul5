void CairoOutputDev::updateMiterLimit(GfxState *state) {
  cairo_set_miter_limit (cairo, state->getMiterLimit());
  if (cairo_shape)
    cairo_set_miter_limit (cairo_shape, state->getMiterLimit());
}
