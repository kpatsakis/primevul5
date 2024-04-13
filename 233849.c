ms_escher_opt_add_str_wchar (GString *buf, gsize marker, GString *extra,
			     guint16 pid, const char *str)
{
	glong ic;
	gunichar2 *str16 = g_utf8_to_utf16 (str, -1, NULL, &ic, NULL);
	guint8 tmp[6];

	GSF_LE_SET_GUINT16 (tmp, pid | 0x8000);
	GSF_LE_SET_GUINT32 (tmp + 2, ic * 2 + 2);
	g_string_append_len (buf, tmp, sizeof tmp);
	g_string_append_len (extra, (gpointer)str16, ic * 2 + 2);

	g_free (str16);

	ms_escher_set_inst (buf, marker,
			    ms_escher_get_inst (buf, marker) + 1);
}