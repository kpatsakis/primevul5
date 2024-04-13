poppler_page_get_image_mapping (PopplerPage *page)
{
  GList *map_list = NULL;
  CairoImageOutputDev *out;
  gint i;
  
  g_return_val_if_fail (POPPLER_IS_PAGE (page), NULL);

  out = poppler_page_get_image_output_dev (page, NULL, NULL);

  for (i = 0; i < out->getNumImages (); i++) {
    PopplerImageMapping *mapping;
    CairoImage *image;

    image = out->getImage (i);

    /* Create the mapping */
    mapping = g_new (PopplerImageMapping, 1);

    image->getRect (&(mapping->area.x1), &(mapping->area.y1),
		    &(mapping->area.x2), &(mapping->area.y2));
    mapping->image_id = i;
    
    mapping->area.x1 -= page->page->getCropBox()->x1;
    mapping->area.x2 -= page->page->getCropBox()->x1;
    mapping->area.y1 -= page->page->getCropBox()->y1;
    mapping->area.y2 -= page->page->getCropBox()->y1;

    map_list = g_list_prepend (map_list, mapping);
  }

  delete out;

  return map_list;	
}
