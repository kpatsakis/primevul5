ms_escher_set_inst (GString *buf, gsize marker, guint inst)
{
	guint16 iv = GSF_LE_GET_GUINT16 (buf->str + marker);
	GSF_LE_SET_GUINT16 (buf->str + marker,
			    (iv & 15) | (inst << 4));
}