ms_escher_opt_add_bool (GString *buf, gsize marker,
			guint16 pid, gboolean b)
{
	int N = ms_escher_get_inst (buf, marker);
	guint16 gid = pid | 0xf;
	guint8 shift = gid - pid;
	guint32 val = (b ? 0x00010001 : 0x00010000) << shift;

	if (N > 0 && GSF_LE_GET_GUINT16 (buf->str + buf->len - 6) == gid) {
		val |= GSF_LE_GET_GUINT32 (buf->str + buf->len - 4);
		GSF_LE_SET_GUINT32 (buf->str + buf->len - 4, val);
	} else
		ms_escher_opt_add_simple (buf, marker, gid, val);
}