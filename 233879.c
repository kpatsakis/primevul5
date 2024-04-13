ms_escher_sp (GString *buf, guint32 spid, guint16 shape, guint32 flags)
{
	gsize marker = buf->len;
	guint8 tmp[16] = {
		0xa2,  0xc, 0xa, 0xf0, 0xde, 0xad, 0xbe, 0xef,
		0, 0, 0, 0, 0, 0, 0, 0
	};
	GSF_LE_SET_GUINT32 (tmp + 4, sizeof (tmp) - 8);
	GSF_LE_SET_GUINT32 (tmp + 8, spid);
	GSF_LE_SET_GUINT32 (tmp + 12, flags);
	g_string_append_len (buf, tmp, sizeof tmp);
	ms_escher_set_inst (buf, marker, shape);
}