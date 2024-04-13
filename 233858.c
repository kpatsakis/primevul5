ms_escher_read_BSE (MSEscherState *state, MSEscherHeader *h)
{
	/* read the header */
	gboolean needs_free;
	guint8 const * data = ms_escher_get_data (state,
		h->offset + COMMON_HEADER_LEN, 36, &needs_free);
	guint8 const  win_type	= GSF_LE_GET_GUINT8  (data + 0);
	guint8 const  mac_type	= GSF_LE_GET_GUINT8  (data + 1);
	/*guint16 const tag	= GSF_LE_GET_GUINT16 (data + 18);*/
	guint32 const size	= GSF_LE_GET_GUINT32 (data + 20);
	guint32 const ref_count	= GSF_LE_GET_GUINT32 (data + 24);
	gint32 const del_offset	= GSF_LE_GET_GUINT32 (data + 28);
	guint8 const is_texture	= GSF_LE_GET_GUINT8  (data + 32);
	guint8 const name_len	= GSF_LE_GET_GUINT8  (data + 33);
	guint8 checksum[16]; /* RSA Data Security, Inc. MD4 Message-Digest Algorithm */
	char const *name = "unknown";
	int i;
	for (i = 16; i-- > 0;)
		checksum[i] = GSF_LE_GET_GUINT8 (data + 2 + i);

	d (0 , {
		g_printerr ("Win type = %s;\n", bliptype_name (win_type));
		g_printerr ("Mac type = %s;\n", bliptype_name (mac_type));
		g_printerr ("Size = 0x%x(=%d) RefCount = 0x%x DelayOffset = 0x%x '%s';\n",
			size, size, ref_count, del_offset, name);

		switch (is_texture) {
		case 0: g_printerr ("Default usage;\n"); break;
		case 1: g_printerr ("Is texture;\n"); break;
		default:g_printerr ("UNKNOWN USAGE : %d;\n", is_texture);
		}

		g_printerr ("Checksum = 0x");
		for (i = 0; i < 16; ++i)
			g_printerr ("%02x", checksum[i]);
		g_printerr (";\n");
	});

	/* Very red herring I think */
	if (name_len != 0) {
		g_printerr ("WARNING : Maybe a name?\n");
		/* name = biff_get_text (data+36, name_len, &txt_byte_len); */
	}

	/* Ignore empties */
	if (h->len > 36 + COMMON_HEADER_LEN)
		return ms_escher_read_container (state, h, 36, FALSE);

	/* Store a blank */
	ms_container_add_blip (state->container, NULL);
	return FALSE;
}