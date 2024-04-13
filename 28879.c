poppler_page_get_text_page (PopplerPage *page)
{
  if (page->text == NULL) {
    cairo_t *cr;
    cairo_surface_t *surface;

    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, 1, 1);
    cr = cairo_create (surface);
    poppler_page_render (page, cr);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);

  }

  return page->text;
}
