poppler_page_render_selection_to_pixbuf (PopplerPage           *page,
                                         gdouble                scale,
                                         int                    rotation,
                                         GdkPixbuf             *pixbuf,
                                         PopplerRectangle      *selection,
                                         PopplerRectangle      *old_selection,
					 PopplerSelectionStyle  style,
                                         GdkColor              *glyph_color,
                                         GdkColor              *background_color)
{
  OutputDev *output_dev;
  OutputDevData data;
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

  poppler_page_prepare_output_dev (page, scale, rotation, TRUE, &data);

#if defined (HAVE_CAIRO)
  TextPage *text;

  text = poppler_page_get_text_page (page);
  text->drawSelection (output_dev, scale, rotation,
			   &pdf_selection, selection_style,
			   &gfx_glyph_color, &gfx_background_color);
#else
  TextOutputDev *text_dev;

  text_dev = poppler_page_get_text_output_dev (page);
  text_dev->drawSelection (output_dev, scale, rotation,
			   &pdf_selection, selection_style,
			   &gfx_glyph_color, &gfx_background_color);
  /* We'll need a function to destroy page->text_dev and page->gfx
   * when the application wants to get rid of them.
   *
   * Two improvements: 1) make GfxFont refcounted and let TextPage and
   * friends hold a reference to the GfxFonts they need so we can free
   * up Gfx early.  2) use a TextPage directly when rendering the page
   * so we don't have to use TextOutputDev and render a second
   * time. */
#endif
  
  poppler_page_copy_to_pixbuf (page, pixbuf, &data);

  poppler_page_set_selection_alpha (page, scale, pixbuf, style, selection);
}
