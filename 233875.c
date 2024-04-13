ms_escher_read_ClientData (MSEscherState *state, MSEscherHeader *h)
{
	guint16 opcode;
	int has_next_record;

	g_return_val_if_fail (h->len == COMMON_HEADER_LEN, TRUE);
	g_return_val_if_fail (h->offset + h->len == state->end_offset, TRUE);

	/* Read the OBJ, be VERY careful until we are sure of the state */
	has_next_record = ms_biff_query_peek_next (state->q, &opcode);
	g_return_val_if_fail (has_next_record, TRUE);
	g_return_val_if_fail (opcode == BIFF_OBJ, TRUE);
	has_next_record = ms_biff_query_next (state->q);
	g_return_val_if_fail (has_next_record, TRUE);

	/* The object takes responsibility for the attributes */
	h->release_attrs = FALSE;
	if (ms_read_OBJ (state->q, state->container, h->attrs)) {
		h->attrs = NULL;  /* It got deleted.  */
		return TRUE;
	}

	return FALSE;
}