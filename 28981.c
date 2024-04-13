void CairoOutputDev::restoreState(GfxState *state) {
  LOG(printf ("restore\n"));
  cairo_restore (cairo);
  if (cairo_shape)
      cairo_restore (cairo_shape);

  /* These aren't restored by cairo_restore() since we keep them in
   * the output device. */
  updateFillColor(state);
  updateStrokeColor(state);
  updateFillOpacity(state);
  updateStrokeOpacity(state);

  MaskStack* ms = maskStack;
  if (mask)
    cairo_pattern_destroy(mask);

  mask = ms->mask;
  maskStack = ms->next;
  delete ms;
}
