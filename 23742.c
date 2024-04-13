vte_terminal_find_charcell (VteTerminal *terminal, glong col, glong row)
{
	VteRowData *rowdata;
	VteCell *ret = NULL;
	VteScreen *screen;
	g_assert(VTE_IS_TERMINAL(terminal));
	screen = terminal->pvt->screen;
	if (_vte_ring_contains (screen->row_data, row)) {
		rowdata = _vte_ring_index_writable (screen->row_data, row);
		ret = _vte_row_data_get_writable (rowdata, col);
	}
	return ret;
}