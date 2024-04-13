comics_document_document_thumbnails_iface_init (EvDocumentThumbnailsInterface *iface)
{
	iface->get_thumbnail = comics_document_thumbnails_get_thumbnail;
	iface->get_dimensions = comics_document_thumbnails_get_dimensions;
}