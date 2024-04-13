poppler_page_get_selection_region (PopplerPage           *page,
				   gdouble                scale,
				   PopplerSelectionStyle  style,
				   PopplerRectangle      *selection)
{
  PDFRectangle poppler_selection;
  SelectionStyle selection_style = selectionStyleGlyph;
  GooList *list;
  GList *region = NULL;
  int i;

  poppler_selection.x1 = selection->x1;
  poppler_selection.y1 = selection->y1;
  poppler_selection.x2 = selection->x2;
  poppler_selection.y2 = selection->y2;

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
  list = text->getSelectionRegion(&poppler_selection,
				  selection_style, scale);
#else
  TextOutputDev *text_dev;
  
  text_dev = poppler_page_get_text_output_dev (page);
  list = text_dev->getSelectionRegion(&poppler_selection,
				      selection_style, scale);
#endif
  
  for (i = 0; i < list->getLength(); i++) {
    PDFRectangle *selection_rect = (PDFRectangle *) list->get(i);
    PopplerRectangle *rect;

    rect = poppler_rectangle_new ();
    
    rect->x1 = selection_rect->x1;
    rect->y1 = selection_rect->y1;
    rect->x2 = selection_rect->x2;
    rect->y2 = selection_rect->y2;
    
    region = g_list_prepend (region, rect);
    
    delete selection_rect;
  }

  delete list;

  return g_list_reverse (region);
}
