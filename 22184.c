comics_document_thumbnails_get_thumbnail (EvDocumentThumbnails *document,
					  EvRenderContext      *rc,
					  gboolean              border)
{
	GdkPixbuf *thumbnail;

	thumbnail = comics_document_render_pixbuf (EV_DOCUMENT (document), rc);

	if (border) {
	      GdkPixbuf *tmp_pixbuf = thumbnail;
	      
	      thumbnail = ev_document_misc_get_thumbnail_frame (-1, -1, tmp_pixbuf);
	      g_object_unref (tmp_pixbuf);
	}

	return thumbnail;
}