poppler_page_copy_to_pixbuf(PopplerPage *page,
			    GdkPixbuf *pixbuf,
			    OutputDevData *data)
{
  SplashOutputDev *output_dev;
  SplashBitmap *bitmap;
  SplashColorPtr color_ptr;
  int splash_width, splash_height, splash_rowstride;
  int pixbuf_rowstride, pixbuf_n_channels;
  guchar *pixbuf_data, *dst;
  int x, y;

  output_dev = page->document->output_dev;

  bitmap = output_dev->getBitmap ();
  color_ptr = bitmap->getDataPtr ();

  splash_width = bitmap->getWidth ();
  splash_height = bitmap->getHeight ();
  splash_rowstride = bitmap->getRowSize ();

  pixbuf_data = gdk_pixbuf_get_pixels (pixbuf);
  pixbuf_rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  pixbuf_n_channels = gdk_pixbuf_get_n_channels (pixbuf);

  if (splash_width > gdk_pixbuf_get_width (pixbuf))
    splash_width = gdk_pixbuf_get_width (pixbuf);
  if (splash_height > gdk_pixbuf_get_height (pixbuf))
    splash_height = gdk_pixbuf_get_height (pixbuf);

  SplashColorPtr pixel = new Guchar[4];
  for (y = 0; y < splash_height; y++)
  {
    dst = pixbuf_data + y * pixbuf_rowstride;
    for (x = 0; x < splash_width; x++)
    {
      output_dev->getBitmap()->getPixel(x, y, pixel);
      dst[0] = pixel[0];
      dst[1] = pixel[1];
      dst[2] = pixel[2];
      if (pixbuf_n_channels == 4)
        dst[3] = 0xff;
      dst += pixbuf_n_channels;
    }
  }
  delete [] pixel;
}
