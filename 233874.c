ms_escher_opt_add_simple (GString *buf, gsize marker, guint16 pid, gint32 val)
{
	guint8 tmp[6];

	GSF_LE_SET_GUINT16 (tmp, pid);
	GSF_LE_SET_GINT32 (tmp + 2, val);
	g_string_append_len (buf, tmp, sizeof tmp);

	ms_escher_set_inst (buf, marker,
			    ms_escher_get_inst (buf, marker) + 1);
}