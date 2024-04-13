void CairoOutputDev::setCairo(cairo_t *cairo)
{
  if (this->cairo != NULL) {
    cairo_status_t status = cairo_status (this->cairo);
    if (status) {
      warning("cairo context error: %s\n", cairo_status_to_string(status));
    }
    cairo_destroy (this->cairo);
    assert(!cairo_shape);
  }
  if (cairo != NULL) {
    this->cairo = cairo_reference (cairo);
	/* save the initial matrix so that we can use it for type3 fonts. */
	cairo_get_matrix(cairo, &orig_matrix);
  } else {
    this->cairo = NULL;
    this->cairo_shape = NULL;
  }
}
