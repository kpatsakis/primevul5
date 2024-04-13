poppler_page_get_size (PopplerPage *page,
		       double      *width,
		       double      *height)
{
  double page_width, page_height;
  int rotate;

  g_return_if_fail (POPPLER_IS_PAGE (page));

  rotate = page->page->getRotate ();
  if (rotate == 90 || rotate == 270) {
    page_height = page->page->getCropWidth ();
    page_width = page->page->getCropHeight ();
  } else {
    page_width = page->page->getCropWidth ();
    page_height = page->page->getCropHeight ();
  }

  if (width != NULL)
    *width = page_width;
  if (height != NULL)
    *height = page_height;
}
