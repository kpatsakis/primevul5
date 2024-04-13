void CairoOutputDev::updateFillOpacity(GfxState *state) {
  fill_opacity = state->getFillOpacity();

  cairo_pattern_destroy(fill_pattern);
  fill_pattern = cairo_pattern_create_rgba(fill_color.r / 65535.0,
					   fill_color.g / 65535.0,
					   fill_color.b / 65535.0,
					   fill_opacity);

  LOG(printf ("fill opacity: %f\n", fill_opacity));
}
