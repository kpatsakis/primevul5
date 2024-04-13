_poppler_page_render_to_pixbuf (PopplerPage *page,
				int src_x, int src_y,
				int src_width, int src_height,
				double scale,
				int rotation,
				GBool printing,
				GdkPixbuf *pixbuf)
{
  OutputDevData data;
  
  poppler_page_prepare_output_dev (page, scale, rotation, FALSE, &data);

  page->page->displaySlice(page->document->output_dev,
			   72.0 * scale, 72.0 * scale,
			   rotation,
			   gFalse, /* useMediaBox */
			   gTrue, /* Crop */
			   src_x, src_y,
			   src_width, src_height,
			   printing,
			   page->document->doc->getCatalog (),
			   NULL, NULL,
			   printing ? poppler_print_annot_cb : NULL, NULL);
  
  poppler_page_copy_to_pixbuf (page, pixbuf, &data);
}
