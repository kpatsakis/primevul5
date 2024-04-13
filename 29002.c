void CairoOutputDev::updateLineWidth(GfxState *state) {
  LOG(printf ("line width: %f\n", state->getLineWidth()));
  if (state->getLineWidth() == 0.0) {
    /* find out how big pixels (device unit) are in the x and y directions
     * choose the smaller of the two as our line width */
    double x = 1.0, y = 1.0;
    cairo_device_to_user_distance(cairo, &x, &y);
    cairo_set_line_width (cairo, MIN(fabs(x),fabs(y)));
  } else {
    cairo_set_line_width (cairo, state->getLineWidth());
  }
  if (cairo_shape)
    cairo_set_line_width (cairo_shape, cairo_get_line_width (cairo));
}
