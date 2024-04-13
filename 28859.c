poppler_page_find_text (PopplerPage *page,
			const char  *text)
{
  PopplerRectangle *match;
  GList *matches;
  double xMin, yMin, xMax, yMax;
  gunichar *ucs4;
  glong ucs4_len;
  double height;
#if defined (HAVE_CAIRO)
  TextPage *text_dev;
#else
  TextOutputDev *text_dev;
#endif
  
  g_return_val_if_fail (POPPLER_IS_PAGE (page), FALSE);
  g_return_val_if_fail (text != NULL, FALSE);

#if defined (HAVE_CAIRO)
  text_dev = poppler_page_get_text_page (page);
#else
  text_dev = new TextOutputDev (NULL, gTrue, gFalse, gFalse);
  page->page->display (text_dev, 72, 72, 0,
		       gFalse, gTrue, gFalse,
		       page->document->doc->getCatalog());
#endif

  ucs4 = g_utf8_to_ucs4_fast (text, -1, &ucs4_len);
  poppler_page_get_size (page, NULL, &height);
  
  matches = NULL;
  xMin = 0;
  yMin = 0;

  while (text_dev->findText (ucs4, ucs4_len,
			     gFalse, gTrue, // startAtTop, stopAtBottom
			     gTrue, gFalse, // startAtLast, stopAtLast
			     gFalse, gFalse, // caseSensitive, backwards
			     &xMin, &yMin, &xMax, &yMax))
    {
      match = g_new (PopplerRectangle, 1);
      match->x1 = xMin;
      match->y1 = height - yMax;
      match->x2 = xMax;
      match->y2 = height - yMin;
      matches = g_list_prepend (matches, match);
    }

#if !defined (HAVE_CAIRO)
  delete text_dev;
#endif
  
  g_free (ucs4);

  return g_list_reverse (matches);
}
