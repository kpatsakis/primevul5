void CairoOutputDev::fill(GfxState *state) {
  doPath (cairo, state, state->getPath());
  cairo_set_fill_rule (cairo, CAIRO_FILL_RULE_WINDING);
  cairo_set_source (cairo, fill_pattern);
  LOG(printf ("fill\n"));
  cairo_fill (cairo);
  if (cairo_shape) {
    cairo_set_fill_rule (cairo_shape, CAIRO_FILL_RULE_WINDING);
    doPath (cairo_shape, state, state->getPath());
    cairo_fill (cairo_shape);
  }
}
