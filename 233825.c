ms_escher_spcontainer_start (GString *buf)
{
	gsize res = buf->len;
	guint8 tmp[8] = { 0xf, 0, 4, 0xf0, 0xde, 0xad, 0xbe, 0xef };
	g_string_append_len (buf, tmp, sizeof tmp);
	return res;
}