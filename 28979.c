void CairoOutputDev::paintTransparencyGroup(GfxState * /*state*/, double * /*bbox*/) {
  cairo_set_source (cairo, group);

  if (!mask) {
    if (shape) {
      cairo_save (cairo);

      /* OPERATOR_SOURCE w/ a mask is defined as (src IN mask) ADD (dest OUT mask)
       * however our source has already been clipped to mask so we only need to
       * do ADD and OUT */

      /* clear the shape mask */
      cairo_set_source (cairo, shape);
      cairo_set_operator (cairo, CAIRO_OPERATOR_DEST_OUT);
      cairo_paint (cairo);

      cairo_set_operator (cairo, CAIRO_OPERATOR_ADD);
      cairo_set_source (cairo, group);
      cairo_paint (cairo);

      cairo_restore (cairo);

      cairo_pattern_destroy (shape);
      shape = NULL;
    } else {
      cairo_paint_with_alpha (cairo, fill_opacity);
    }
    cairo_status_t status = cairo_status(cairo);
    if (status)
      printf("BAD status: %s\n", cairo_status_to_string(status));
  } else {
    cairo_mask(cairo, mask);

    cairo_pattern_destroy(mask);
    mask = NULL;
  }

  popTransparencyGroup();
}
