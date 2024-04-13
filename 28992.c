void CairoOutputDev::stroke(GfxState *state) {
  doPath (cairo, state, state->getPath());
  cairo_set_source (cairo, stroke_pattern);
  LOG(printf ("stroke\n"));
  cairo_stroke (cairo);
  if (cairo_shape) {
    doPath (cairo_shape, state, state->getPath());
    cairo_stroke (cairo_shape);
  }
}
