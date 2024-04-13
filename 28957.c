void CairoOutputDev::beginTransparencyGroup(GfxState * /*state*/, double * /*bbox*/,
                                      GfxColorSpace * blendingColorSpace,
                                      GBool /*isolated*/, GBool knockout,
				      GBool forSoftMask) {
  /* push color space */
  ColorSpaceStack* css = new ColorSpaceStack;
  css->cs = blendingColorSpace;
  css->knockout = knockout;
  css->next = groupColorSpaceStack;
  groupColorSpaceStack = css;
  if (knockout) {
    knockoutCount++;
    if (!cairo_shape) {
      /* create a surface for tracking the shape */
      cairo_surface_t *cairo_shape_surface = cairo_surface_create_similar_clip (cairo, CAIRO_CONTENT_ALPHA);
      cairo_shape = cairo_create (cairo_shape_surface);
      cairo_surface_destroy (cairo_shape_surface);

      /* the color doesn't matter as long as it is opaque */
      cairo_set_source_rgb (cairo_shape, 0, 0, 0);
      cairo_matrix_t matrix;
      cairo_get_matrix (cairo, &matrix);
      cairo_set_matrix (cairo_shape, &matrix);
    } else {
      cairo_reference (cairo_shape);
    }
  }
  if (groupColorSpaceStack->next && groupColorSpaceStack->next->knockout) {
    /* we need to track the shape */
    cairo_push_group (cairo_shape);
  }
  if (0 && forSoftMask)
    cairo_push_group_with_content (cairo, CAIRO_CONTENT_ALPHA);
  else
    cairo_push_group (cairo);

  /* push_group has an implicit cairo_save() */
  if (knockout) {
    /*XXX: let's hope this matches the semantics needed */
    cairo_set_operator(cairo, CAIRO_OPERATOR_SOURCE);
  } else {
    cairo_set_operator(cairo, CAIRO_OPERATOR_OVER);
  }
}
