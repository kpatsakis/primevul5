ms_escher_read_ClientAnchor (MSEscherState *state, MSEscherHeader *h)
{
	gboolean needs_free;
	guint8 const *data;

	g_return_val_if_fail (state != NULL, TRUE);
	g_return_val_if_fail (state->container != NULL, TRUE);

	data = ms_escher_get_data (state, h->offset + COMMON_HEADER_LEN,
		MS_ANCHOR_SIZE, &needs_free);
#if 0
	gsf_mem_dump (data, MS_ANCHOR_SIZE);
#endif
	if (data) {
		guint8 *anchor = g_memdup (data, MS_ANCHOR_SIZE);

		ms_escher_header_add_attr (h,
			ms_obj_attr_new_ptr (MS_OBJ_ATTR_ANCHOR, anchor));

		if (needs_free)
			g_free ((guint8 *)data);
		return FALSE;
	}

	return TRUE;
}