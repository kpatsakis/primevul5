CairoOutputDev::~CairoOutputDev() {
  if (fontEngine_owner && fontEngine) {
    delete fontEngine;
  }

  if (cairo)
    cairo_destroy (cairo);
  cairo_pattern_destroy (stroke_pattern);
  cairo_pattern_destroy (fill_pattern);
  if (group)
    cairo_pattern_destroy (group);
  if (mask)
    cairo_pattern_destroy (mask);
  if (shape)
    cairo_pattern_destroy (shape);
  if (text) 
    text->decRefCnt();
  if (actualText)
    delete actualText;  
}
