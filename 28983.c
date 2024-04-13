void CairoOutputDev::saveState(GfxState *state) {
  LOG(printf ("save\n"));
  cairo_save (cairo);
  if (cairo_shape)
      cairo_save (cairo_shape);

  MaskStack *ms = new MaskStack;
  ms->mask = cairo_pattern_reference(mask);
  ms->next = maskStack;
  maskStack = ms;
}
