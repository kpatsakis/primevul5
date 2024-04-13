vte_sequence_handler_set_scrolling_region_to_end (VteTerminal *terminal, GValueArray *params)
{
	GValue value = {0};

	g_value_init (&value, G_TYPE_LONG);
	g_value_set_long (&value, 0); /* Out of range means start/end */

	g_value_array_insert (params, 1, &value);

	vte_sequence_handler_offset(terminal, params, -1, vte_sequence_handler_cs);
}