vte_sequence_handler_cd (VteTerminal *terminal, GValueArray *params)
{
	VteRowData *rowdata;
	glong i;
	VteScreen *screen;

	screen = terminal->pvt->screen;
	/* If the cursor is actually on the screen, clear the rest of the
	 * row the cursor is on and all of the rows below the cursor. */
	i = screen->cursor_current.row;
	if (i < _vte_ring_next(screen->row_data)) {
		/* Get the data for the row we're clipping. */
		rowdata = _vte_ring_index_writable (screen->row_data, i);
		/* Clear everything to the right of the cursor. */
		if (rowdata)
			_vte_row_data_shrink (rowdata, screen->cursor_current.col);
	}
	/* Now for the rest of the lines. */
	for (i = screen->cursor_current.row + 1;
	     i < _vte_ring_next(screen->row_data);
	     i++) {
		/* Get the data for the row we're removing. */
		rowdata = _vte_ring_index_writable (screen->row_data, i);
		/* Remove it. */
		if (rowdata)
			_vte_row_data_shrink (rowdata, 0);
	}
	/* Now fill the cleared areas. */
	for (i = screen->cursor_current.row;
	     i < screen->insert_delta + terminal->row_count;
	     i++) {
		/* Retrieve the row's data, creating it if necessary. */
		if (_vte_ring_contains (screen->row_data, i)) {
			rowdata = _vte_ring_index_writable (screen->row_data, i);
			g_assert(rowdata != NULL);
		} else {
			rowdata = _vte_terminal_ring_append (terminal, FALSE);
		}
		/* Pad out the row. */
		_vte_row_data_fill (rowdata, &screen->fill_defaults, terminal->column_count);
		rowdata->attr.soft_wrapped = 0;
		/* Repaint this row. */
		_vte_invalidate_cells(terminal,
				      0, terminal->column_count,
				      i, 1);
	}

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_deleted_flag = TRUE;
}