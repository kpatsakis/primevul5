poppler_page_render (PopplerPage *page,
		     cairo_t *cairo)
{
  g_return_if_fail (POPPLER_IS_PAGE (page));

  if (!page->text)
    page->text = new TextPage(gFalse);

  _poppler_page_render (page, cairo, gFalse);
}
