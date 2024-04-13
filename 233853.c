ms_escher_get_inst (GString *buf, gsize marker)
{
	guint16 iv = GSF_LE_GET_GUINT16 (buf->str + marker);
	return (iv >> 4);
}