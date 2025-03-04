exif_data_load_data_content (ExifData *data, ExifIfd ifd,
			     const unsigned char *d,
			     unsigned int ds, unsigned int offset, unsigned int recursion_cost)
{
	ExifLong o, thumbnail_offset = 0, thumbnail_length = 0;
	ExifShort n;
	ExifEntry *entry;
	unsigned int i;
	ExifTag tag;

	if (!data || !data->priv) 
		return;

	/* check for valid ExifIfd enum range */
	if ((((int)ifd) < 0) || ( ((int)ifd) >= EXIF_IFD_COUNT))
	  return;

	if (recursion_cost > 170) {
		/*
		 * recursion_cost is a logarithmic-scale indicator of how expensive this
		 * recursive call might end up being. It is an indicator of the depth of
		 * recursion as well as the potential for worst-case future recursive
		 * calls. Since it's difficult to tell ahead of time how often recursion
		 * will occur, this assumes the worst by assuming every tag could end up
		 * causing recursion.
		 * The value of 170 was chosen to limit typical EXIF structures to a
		 * recursive depth of about 6, but pathological ones (those with very
		 * many tags) to only 2.
		 */
		exif_log (data->priv->log, EXIF_LOG_CODE_CORRUPT_DATA, "ExifData",
			  "Deep/expensive recursion detected!");
		return;
	}

	/* Read the number of entries */
	if (CHECKOVERFLOW(offset, ds, 2)) {
		exif_log (data->priv->log, EXIF_LOG_CODE_CORRUPT_DATA, "ExifData",
			  "Tag data past end of buffer (%u+2 > %u)", offset, ds);
		return;
	}
	n = exif_get_short (d + offset, data->priv->order);
	exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
	          "Loading %hu entries...", n);
	offset += 2;

	/* Check if we have enough data. */
	if (CHECKOVERFLOW(offset, ds, 12*n)) {
		n = (ds - offset) / 12;
		exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
				  "Short data; only loading %hu entries...", n);
	}

	for (i = 0; i < n; i++) {

		tag = exif_get_short (d + offset + 12 * i, data->priv->order);
		switch (tag) {
		case EXIF_TAG_EXIF_IFD_POINTER:
		case EXIF_TAG_GPS_INFO_IFD_POINTER:
		case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
		case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
		case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
			o = exif_get_long (d + offset + 12 * i + 8,
					   data->priv->order);
			if (o >= ds) {
				exif_log (data->priv->log, EXIF_LOG_CODE_CORRUPT_DATA, "ExifData",
					  "Tag data past end of buffer (%u > %u)", offset+2, ds);
				return;
			}
			/* FIXME: IFD_POINTER tags aren't marked as being in a
			 * specific IFD, so exif_tag_get_name_in_ifd won't work
			 */
			exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
				  "Sub-IFD entry 0x%x ('%s') at %u.", tag,
				  exif_tag_get_name(tag), o);
			switch (tag) {
			case EXIF_TAG_EXIF_IFD_POINTER:
				CHECK_REC (EXIF_IFD_EXIF);
				exif_data_load_data_content (data, EXIF_IFD_EXIF, d, ds, o,
					recursion_cost + level_cost(n));
				break;
			case EXIF_TAG_GPS_INFO_IFD_POINTER:
				CHECK_REC (EXIF_IFD_GPS);
				exif_data_load_data_content (data, EXIF_IFD_GPS, d, ds, o,
					recursion_cost + level_cost(n));
				break;
			case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
				CHECK_REC (EXIF_IFD_INTEROPERABILITY);
				exif_data_load_data_content (data, EXIF_IFD_INTEROPERABILITY, d, ds, o,
					recursion_cost + level_cost(n));
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
				thumbnail_offset = o;
				if (thumbnail_offset && thumbnail_length)
					exif_data_load_data_thumbnail (data, d,
								       ds, thumbnail_offset,
								       thumbnail_length);
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
				thumbnail_length = o;
				if (thumbnail_offset && thumbnail_length)
					exif_data_load_data_thumbnail (data, d,
								       ds, thumbnail_offset,
								       thumbnail_length);
				break;
			default:
				return;
			}
			break;
		default:

			/*
			 * If we don't know the tag, don't fail. It could be that new 
			 * versions of the standard have defined additional tags. Note that
			 * 0 is a valid tag in the GPS IFD.
			 */
			if (!exif_tag_get_name_in_ifd (tag, ifd)) {

				/*
				 * Special case: Tag and format 0. That's against specification
				 * (at least up to 2.2). But Photoshop writes it anyways.
				 */
				if (!memcmp (d + offset + 12 * i, "\0\0\0\0", 4)) {
					exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
						  "Skipping empty entry at position %u in '%s'.", i, 
						  exif_ifd_get_name (ifd));
					break;
				}
				exif_log (data->priv->log, EXIF_LOG_CODE_DEBUG, "ExifData",
					  "Unknown tag 0x%04x (entry %u in '%s'). Please report this tag "
					  "to <libexif-devel@lists.sourceforge.net>.", tag, i,
					  exif_ifd_get_name (ifd));
				if (data->priv->options & EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS)
					break;
			}
			entry = exif_entry_new_mem (data->priv->mem);
			if (!entry) {
				  exif_log (data->priv->log, EXIF_LOG_CODE_NO_MEMORY, "ExifData",
                                          "Could not allocate memory");
				  return;
			}
			if (exif_data_load_data_entry (data, entry, d, ds,
						   offset + 12 * i))
				exif_content_add_entry (data->ifd[ifd], entry);
			exif_entry_unref (entry);
			break;
		}
	}
}