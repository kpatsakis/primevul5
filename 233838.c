ms_escher_read_ClientTextbox (MSEscherState *state, MSEscherHeader *h)
{
	guint16 opcode;
	int has_next_record;
	char *text;
	PangoAttrList *markup;

	g_return_val_if_fail (h->len == COMMON_HEADER_LEN, TRUE);
	g_return_val_if_fail (h->offset + h->len == state->end_offset, TRUE);

	/* Read the TXO, be VERY careful until we are sure of the state */
	has_next_record = ms_biff_query_peek_next (state->q, &opcode);
	g_return_val_if_fail (has_next_record, TRUE);
	g_return_val_if_fail (opcode == BIFF_TXO, TRUE);
	has_next_record = ms_biff_query_next (state->q);
	g_return_val_if_fail (has_next_record, TRUE);

	text = ms_read_TXO (state->q, state->container, &markup);
	ms_escher_header_add_attr (h,
		ms_obj_attr_new_ptr (MS_OBJ_ATTR_TEXT, text));
	if (markup != NULL) {
		ms_escher_header_add_attr (h,
			ms_obj_attr_new_markup (MS_OBJ_ATTR_MARKUP, markup));
		pango_attr_list_unref (markup);
	}
	d (0, g_printerr ("'%s';\n", text););
	return FALSE;
}