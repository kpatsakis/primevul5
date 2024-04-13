poppler_page_render_selection (PopplerPage           *page,
			       cairo_t               *cairo,
			       PopplerRectangle      *selection,
			       PopplerRectangle      *old_selection,
			       PopplerSelectionStyle  style, 
			       PopplerColor          *glyph_color,
			       PopplerColor          *background_color)
{
  CairoOutputDev *output_dev;
  TextPage *text;
  SelectionStyle selection_style = selectionStyleGlyph;
  PDFRectangle pdf_selection(selection->x1, selection->y1,
			     selection->x2, selection->y2);

  GfxColor gfx_background_color = {
      {
	  background_color->red,
	  background_color->green,
	  background_color->blue
      }
  };
  GfxColor gfx_glyph_color = {
      {
	  glyph_color->red,
	  glyph_color->green,
	  glyph_color->blue
      }
  };

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

  output_dev = page->document->output_dev;
  output_dev->setCairo (cairo);

  text = poppler_page_get_text_page (page);
  text->drawSelection (output_dev, 1.0, 0,
		       &pdf_selection, selection_style,
		       &gfx_glyph_color, &gfx_background_color);

  output_dev->setCairo (NULL);
}
