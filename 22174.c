comics_document_thumbnails_get_dimensions (EvDocumentThumbnails *document,
					   EvRenderContext      *rc,
					   gint                 *width,
					   gint                 *height)
{
	gdouble page_width, page_height;
	
	comics_document_get_page_size (EV_DOCUMENT (document), rc->page,
				       &page_width, &page_height);

	if (rc->rotation == 90 || rc->rotation == 270) {
		*width = (gint) (page_height * rc->scale);
		*height = (gint) (page_width * rc->scale);
	} else {
		*width = (gint) (page_width * rc->scale);
		*height = (gint) (page_height * rc->scale);
	}
}