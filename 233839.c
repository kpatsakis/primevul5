ms_escher_read_Blip (MSEscherState *state, MSEscherHeader *h)
{
	int offset = COMMON_HEADER_LEN + 16;
	guint32 inst = h->instance;
	gboolean needs_free, failure = FALSE;
	guint8 const *data;
	MSEscherBlip *blip = NULL;
	char const   *type = NULL;

	/*  This doesn't make alot of sense.
	 *  Which is the normative indicator of what type the blip is ?
	 *  We have the requested type for each OS, and the instance code for
	 *  this record.  The magic number for this record seems to be the right
	 *  things to use, but the comments from S59FE3.HTM
	 *
	 *  NOTE!: The primary UID is only saved to disk
	 *     if (inst ^ blip_signature == 1).
	 *
	 * suggest that sometimes the magic numers have built flags.
	 * I'll assume that what they mean to say is that the magic number
	 * in the instance code may have its low bit set to indicate the
	 * presence of a primary uid if the rest of the signature is one
	 * of the known signatures.
	 */
	if (inst & 0x1) {
		offset += 16;
		inst &= (~0x1);
	}

	/* Clients may set bit 0x800 */
	inst &= (~0x800);
	if (inst == 0x216 || inst == 0x3d4 || inst == 0x542) {
		guint8 const *tmp = ms_escher_get_data (state, h->offset + offset,
			META_FILE_HEADER_LEN, &needs_free);
		guint32 uncompressed_len = GSF_LE_GET_GUINT32 (tmp+0);
		guint32 compressed_len = GSF_LE_GET_GUINT32 (tmp+28);
		guint8  compress = tmp[32];
		guint8  filter	 = tmp[33];

		if (needs_free)
			g_free ((guint8*)tmp);
		offset += META_FILE_HEADER_LEN;

		if (inst == 0x216)
			type = "wmf";
		else if (inst == 0x3d4)
			type = "emf";
		else
			type = "pict";

		if (filter != 0xfe /* must be none */	||
		    (unsigned)(h->len - offset) < compressed_len) {
			failure = TRUE;
			g_warning ("invalid metafile header %x, %u != %u;", filter,
				   (h->len - offset), compressed_len);
		} else {
			tmp = ms_escher_get_data (state, h->offset + offset,
				compressed_len, &needs_free);
			if (compress == 0) {		/* Yes 0 == deflate */
				uLongf len = uncompressed_len*4;
				guint8 *buffer = g_malloc (len);

				int res = uncompress (buffer, &len, tmp, compressed_len);
				if (res != Z_OK) {
					g_free (buffer);
					failure = TRUE;
					g_warning ("compression failure %d;", res);
				} else
					blip = ms_escher_blip_new (buffer,
						len, type, FALSE);
			} else if (compress == 0xfe) {	/* 0xfe  == none */
				blip = ms_escher_blip_new ((guint8 *)tmp, compressed_len,
					type, !needs_free);
				needs_free = FALSE;
			} else {
				failure = TRUE;
				g_warning ("Unknown compression type %hhx;", compress);
			}
			if (needs_free)
				g_free ((guint8*)tmp);
		}
	} else if (inst == 0x46a || inst == 0x6e0 || inst == 0x7a8) {
		if (inst == 0x46a)	/* JPEG data, with 1 byte header */
			type = "jpeg";
		else if (inst == 0x6e0)	/* PNG  data, with 1 byte header */
			type = "png";
		else			/* DIB  data, with 1 byte header */
			type = "dib";
		offset++;
		data = ms_escher_get_data (state, h->offset + offset,
			h->len - offset, &needs_free);
		if (data)
			blip = ms_escher_blip_new ((guint8 *)data,
						   h->len - offset,
						   type, !needs_free);
		else
			failure = TRUE;
	} else {
		failure = TRUE;
		g_warning ("Don't know what to do with this image %x;", inst);
	}

	/* Always add a blip to keep the indicies in sync, even if it's NULL */
	ms_container_add_blip (state->container, blip);
	return failure;
}