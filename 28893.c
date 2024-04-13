poppler_page_set_selection_alpha (PopplerPage           *page,
				  double                 scale,
				  GdkPixbuf             *pixbuf,
				  PopplerSelectionStyle  style,
				  PopplerRectangle      *selection)
{
  /* Cairo doesn't need this, since cairo generates an alpha channel. */ 
}
