void CairoOutputDev::startPage(int pageNum, GfxState *state) {
  /* set up some per page defaults */
  cairo_pattern_destroy(fill_pattern);
  fill_pattern = cairo_pattern_create_rgb(0., 0., 0.);

  cairo_pattern_destroy(stroke_pattern);
  stroke_pattern = cairo_pattern_create_rgb(0., 0., 0.);

  if (text)
    text->startPage(state);
}
