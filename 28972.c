void CairoOutputDev::endTextObject(GfxState *state) {
  if (textClipPath) {
    cairo_append_path (cairo, textClipPath);
    cairo_clip (cairo);
    if (cairo_shape) {
      cairo_append_path (cairo_shape, textClipPath);
      cairo_clip (cairo_shape);
    }
    cairo_path_destroy (textClipPath);
    textClipPath = NULL;
  }
}
