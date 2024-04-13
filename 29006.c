CairoImage::~CairoImage () {
  if (image)
    cairo_surface_destroy (image);
}
