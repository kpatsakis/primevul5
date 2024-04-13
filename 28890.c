poppler_page_render_to_pixbuf (PopplerPage *page,
			       int src_x, int src_y,
			       int src_width, int src_height,
			       double scale,
			       int rotation,
			       GdkPixbuf *pixbuf)
{
  g_return_if_fail (POPPLER_IS_PAGE (page));
  g_return_if_fail (scale > 0.0);
  g_return_if_fail (pixbuf != NULL);

  _poppler_page_render_to_pixbuf (page, src_x, src_y,
				  src_width, src_height,
				  scale, rotation,
				  gFalse,
				  pixbuf);
}
