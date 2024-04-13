poppler_page_get_crop_box (PopplerPage *page, PopplerRectangle *rect)
{
  PDFRectangle* cropBox = page->page->getCropBox ();
  
  rect->x1 = cropBox->x1;
  rect->x2 = cropBox->x2;
  rect->y1 = cropBox->y1;
  rect->y2 = cropBox->y2;
}
