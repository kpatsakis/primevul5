void CairoOutputDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
				    int width, int height, GBool invert,
				    GBool inlineImg) {

  /* FIXME: Doesn't the image mask support any colorspace? */
  cairo_set_source (cairo, fill_pattern);

  /* work around a cairo bug when scaling 1x1 surfaces */
  if (width == 1 && height == 1) {
    cairo_save (cairo);
    cairo_rectangle (cairo, 0., 0., width, height);
    cairo_fill (cairo);
    cairo_restore (cairo);
    if (cairo_shape) {
      cairo_save (cairo_shape);
      cairo_rectangle (cairo_shape, 0., 0., width, height);
      cairo_fill (cairo_shape);
      cairo_restore (cairo_shape);
    }
    return;
  }

  /* shape is 1.0 for painted areas, 0.0 for unpainted ones */

  cairo_matrix_t matrix;
  cairo_get_matrix (cairo, &matrix);
  if (!printing && prescaleImages && matrix.xy == 0.0 && matrix.yx == 0.0) {
    drawImageMaskPrescaled(state, ref, str, width, height, invert, inlineImg);
  } else {
    drawImageMaskRegular(state, ref, str, width, height, invert, inlineImg);
  }
}
