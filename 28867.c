poppler_page_get_duration (PopplerPage *page)
{
  g_return_val_if_fail (POPPLER_IS_PAGE (page), -1);

  return page->page->getDuration ();
}
