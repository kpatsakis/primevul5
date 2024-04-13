ms_escher_clientdata (GString *buf)
{
	guint8 tmp[8] = { 0, 0, 0x11, 0xf0, 0xde, 0xad, 0xbe, 0xef };
	GSF_LE_SET_GUINT32 (tmp + 4, 0);
	g_string_append_len (buf, tmp, sizeof tmp);
}