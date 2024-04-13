poppler_page_render_for_printing (PopplerPage *page,
				  cairo_t *cairo)
{
  g_return_if_fail (POPPLER_IS_PAGE (page));
  
  _poppler_page_render (page, cairo, gTrue);	
}
