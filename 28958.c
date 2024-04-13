GBool CairoOutputDev::beginType3Char(GfxState *state, double x, double y,
				      double dx, double dy,
				      CharCode code, Unicode *u, int uLen) {

  cairo_save (cairo);
  double *ctm;
  cairo_matrix_t matrix;

  ctm = state->getCTM();
  matrix.xx = ctm[0];
  matrix.yx = ctm[1];
  matrix.xy = ctm[2];
  matrix.yy = ctm[3];
  matrix.x0 = ctm[4];
  matrix.y0 = ctm[5];
  /* Restore the original matrix and then transform to matrix needed for the
   * type3 font. This is ugly but seems to work. Perhaps there is a better way to do it?*/
  cairo_set_matrix(cairo, &orig_matrix);
  cairo_transform(cairo, &matrix);
  if (cairo_shape) {
    cairo_save (cairo_shape);
    cairo_set_matrix(cairo_shape, &orig_matrix);
    cairo_transform(cairo_shape, &matrix);
  }
  cairo_pattern_destroy(stroke_pattern);
  cairo_pattern_reference(fill_pattern);
  stroke_pattern = fill_pattern;
  return gFalse;
}
