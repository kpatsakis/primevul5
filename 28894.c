poppler_page_set_selection_alpha (PopplerPage           *page,
				  double                 scale,
				  GdkPixbuf             *pixbuf,
				  PopplerSelectionStyle  style,
				  PopplerRectangle      *selection)
{
  GList *region, *l;
  gint x, y, width, height;
  int pixbuf_rowstride, pixbuf_n_channels;
  guchar *pixbuf_data, *dst;

  pixbuf_data = gdk_pixbuf_get_pixels (pixbuf);
  pixbuf_rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  pixbuf_n_channels = gdk_pixbuf_get_n_channels (pixbuf);
  width = gdk_pixbuf_get_width (pixbuf);
  height = gdk_pixbuf_get_height (pixbuf);

  if (pixbuf_n_channels != 4)
    return;

  for (y = 0; y < height; y++) {
      dst = pixbuf_data + y * pixbuf_rowstride;
      for (x = 0; x < width; x++) {
	  dst[3] = 0x00;
	  dst += pixbuf_n_channels;
      }
  }

  region = poppler_page_get_selection_region (page, scale, style, selection);

  for (l = region; l; l = g_list_next (l)) {
    PopplerRectangle *rectangle = (PopplerRectangle *)l->data;
    GdkRectangle rect;
    
    rect.x = (gint)rectangle->x1;
    rect.y = (gint)rectangle->y1;
    rect.width  = (gint) (rectangle->x2 - rectangle->x1);
    rect.height = (gint) (rectangle->y2 - rectangle->y1);
    
    for (y = 0; y < rect.height; y++) {
      dst = pixbuf_data + (rect.y + y) * pixbuf_rowstride +
	rect.x * pixbuf_n_channels;
      for (x = 0; x < rect.width; x++) {
	  dst[3] = 0xff;
	  dst += pixbuf_n_channels;
      }
    }
  }

  poppler_page_selection_region_free (region);
}
