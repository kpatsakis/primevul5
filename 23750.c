vte_sequence_handler_uc (VteTerminal *terminal, GValueArray *params)
{
	VteCell *cell;
	int column;
	VteScreen *screen;

	screen = terminal->pvt->screen;
	column = screen->cursor_current.col;
	cell = vte_terminal_find_charcell(terminal, column, screen->cursor_current.row);
	while ((cell != NULL) && (cell->attr.fragment) && (column > 0)) {
		column--;
		cell = vte_terminal_find_charcell(terminal, column, screen->cursor_current.row);
	}
	if (cell != NULL) {
		/* Set this character to be underlined. */
		cell->attr.underline = 1;
		/* Cause the character to be repainted. */
		_vte_invalidate_cells(terminal,
				      column, cell->attr.columns,
				      screen->cursor_current.row, 1);
		/* Move the cursor right. */
		vte_sequence_handler_nd (terminal, params);
	}

	/* We've modified the display without changing the text.  Make a note
	 * of it. */
	terminal->pvt->text_modified_flag = TRUE;
}