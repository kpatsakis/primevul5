ms_escher_opt_add_color (GString *buf, gsize marker,
			 guint16 pid, GOColor c)
{
	guint8 r = GO_COLOR_UINT_R (c);
	guint8 g = GO_COLOR_UINT_G (c);
	guint8 b = GO_COLOR_UINT_B (c);
	guint32 val = r | (g << 8) | (b << 16);
	ms_escher_opt_add_simple (buf, marker, pid, val);
}