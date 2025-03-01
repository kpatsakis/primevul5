exif_data_load_data_thumbnail (ExifData *data, const unsigned char *d,
			       unsigned int ds, ExifLong o, ExifLong s)
{
	/* Sanity checks */
	if (o >= ds) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData", "Bogus thumbnail offset (%u).", o);
		return;
	}
	if (CHECKOVERFLOW(o,ds,s)) {
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData", "Bogus thumbnail size (%u), max would be %u.", s, ds-o);
		return;
	}
	if (data->data) 
		exif_mem_free (data->priv->mem, data->data);
	if (!(data->data = exif_data_alloc (data, s))) {
		EXIF_LOG_NO_MEMORY (data->priv->log, "ExifData", s);
		data->size = 0;
		return;
	}
	data->size = s;
	memcpy (data->data, d + o, s);
}