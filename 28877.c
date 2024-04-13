poppler_page_get_text (PopplerPage          *page,
		       PopplerSelectionStyle style,
		       PopplerRectangle     *selection)
{
  GooString *sel_text;
  double height;
  char *result;
  SelectionStyle selection_style = selectionStyleGlyph;
  PDFRectangle pdf_selection;

  g_return_val_if_fail (POPPLER_IS_PAGE (page), FALSE);
  g_return_val_if_fail (selection != NULL, NULL);

  poppler_page_get_size (page, NULL, &height);
  pdf_selection.x1 = selection->x1;
  pdf_selection.y1 = height - selection->y2;
  pdf_selection.x2 = selection->x2;
  pdf_selection.y2 = height - selection->y1;

  switch (style)
    {
      case POPPLER_SELECTION_GLYPH:
        selection_style = selectionStyleGlyph;
	break;
      case POPPLER_SELECTION_WORD:
        selection_style = selectionStyleWord;
	break;
      case POPPLER_SELECTION_LINE:
        selection_style = selectionStyleLine;
	break;
    }

#if defined (HAVE_CAIRO)
  TextPage *text;

  text = poppler_page_get_text_page (page);
  sel_text = text->getSelectionText (&pdf_selection, selection_style);
#else
  TextOutputDev *text_dev;

  text_dev = poppler_page_get_text_output_dev (page);
  sel_text = text_dev->getSelectionText (&pdf_selection, selection_style);
#endif
	  
  result = g_strdup (sel_text->getCString ());
  delete sel_text;

  return result;
}
