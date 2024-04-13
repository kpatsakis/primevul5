poppler_page_get_image (PopplerPage *page,
			gint         image_id)
{
  CairoImageOutputDev *out;
  cairo_surface_t *image;
  
  g_return_val_if_fail (POPPLER_IS_PAGE (page), NULL);

  out = poppler_page_get_image_output_dev (page,
					   image_draw_decide_cb,
					   GINT_TO_POINTER (image_id));

  if (image_id >= out->getNumImages ()) {
    delete out;
    
    return NULL;
  }

  image = out->getImage (image_id)->getImage ();
  if (!image) {
    delete out;

    return NULL;
  }

  cairo_surface_reference (image);
  delete out;
  
  return image;
}
