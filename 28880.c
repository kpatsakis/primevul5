poppler_page_get_thumbnail (PopplerPage *page)
{
  unsigned char *data;
  int width, height, rowstride;
  cairo_surface_t *surface;

  g_return_val_if_fail (POPPLER_IS_PAGE (page), FALSE);

  if (!page->page->loadThumb (&data, &width, &height, &rowstride))
    return NULL;

  surface = create_surface_from_thumbnail_data (data, width, height, rowstride);
  gfree (data);
  
  return surface;
}
