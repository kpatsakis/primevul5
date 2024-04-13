poppler_page_get_image_output_dev (PopplerPage *page,
				   GBool (*imgDrawDeviceCbk)(int img_id, void *data),
				   void *imgDrawCbkData)
{
  CairoImageOutputDev *image_dev;
  Gfx *gfx;
  
  image_dev = new CairoImageOutputDev ();

  if (imgDrawDeviceCbk) {
    image_dev->setImageDrawDecideCbk (imgDrawDeviceCbk,
				      imgDrawCbkData);
  }

  gfx = page->page->createGfx(image_dev,
			      72.0, 72.0, 0,
			      gFalse, /* useMediaBox */
			      gTrue, /* Crop */
			      -1, -1, -1, -1,
			      gFalse, /* printing */
			      page->document->doc->getCatalog (),
			      NULL, NULL, NULL, NULL);
  page->page->display(gfx);
  delete gfx;

  return image_dev;
}
