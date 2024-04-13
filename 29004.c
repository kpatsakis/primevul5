void CairoOutputDev::updateStrokeColor(GfxState *state) {
  state->getStrokeRGB(&stroke_color);

  cairo_pattern_destroy(stroke_pattern);
  stroke_pattern = cairo_pattern_create_rgba(stroke_color.r / 65535.0,
					     stroke_color.g / 65535.0,
					     stroke_color.b / 65535.0,
					     stroke_opacity);
  
  LOG(printf ("stroke color: %d %d %d\n",
	      stroke_color.r, stroke_color.g, stroke_color.b));
}
