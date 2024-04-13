void CairoOutputDev::endTransparencyGroup(GfxState * /*state*/) {

  if (group)
    cairo_pattern_destroy(group);
  group = cairo_pop_group (cairo);

  if (groupColorSpaceStack->next && groupColorSpaceStack->next->knockout) {
    if (shape)
      cairo_pattern_destroy(shape);
    shape = cairo_pop_group (cairo_shape);
  }
}
