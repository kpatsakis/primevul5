ms_escher_opt_start (GString *buf)
{
	gsize res = buf->len;
	guint8 tmp[8] = { 0x03,   0, 0xb, 0xf0, 0xde, 0xad, 0xbe, 0xef };
	g_string_append_len (buf, tmp, sizeof tmp);
	return res;
}