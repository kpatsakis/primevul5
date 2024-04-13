ms_escher_read_SplitMenuColors (MSEscherState *state, MSEscherHeader *h)
{
	gboolean needs_free;
	guint8 const * data;

	g_return_val_if_fail (h->instance == 4, TRUE);
	g_return_val_if_fail (h->len == 24, TRUE); /*header + 4*4 */

	if ((data = ms_escher_get_data (state, h->offset + COMMON_HEADER_LEN,
					16, &needs_free))) {
		guint32 const top_level_fill = GSF_LE_GET_GUINT32(data + 0);
		guint32 const line	= GSF_LE_GET_GUINT32(data + 4);
		guint32 const shadow	= GSF_LE_GET_GUINT32(data + 8);
		guint32 const threeD	= GSF_LE_GET_GUINT32(data + 12);

		d (0, g_printerr ("top_level_fill = 0x%x;\nline = 0x%x;\nshadow = 0x%x;\nthreeD = 0x%x;\n",
			      top_level_fill, line, shadow, threeD););
	} else
		return TRUE;
	return FALSE;
}