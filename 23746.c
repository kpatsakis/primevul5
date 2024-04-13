vte_sequence_handler_ta (VteTerminal *terminal, GValueArray *params)
{
	VteScreen *screen;
	long old_len, newcol, col;

	/* Calculate which column is the next tab stop. */
	screen = terminal->pvt->screen;
	newcol = col = screen->cursor_current.col;

	g_assert (col >= 0);

	if (terminal->pvt->tabstops != NULL) {
		/* Find the next tabstop. */
		for (newcol++; newcol < VTE_TAB_MAX; newcol++) {
			if (_vte_terminal_get_tabstop(terminal, newcol)) {
				break;
			}
		}
	}

	/* If we have no tab stops or went past the end of the line, stop
	 * at the right-most column. */
	if (newcol >= terminal->column_count) {
		newcol = terminal->column_count - 1;
	}

	/* but make sure we don't move cursor back (bug #340631) */
	if (col < newcol) {
		VteRowData *rowdata = _vte_terminal_ensure_row (terminal);

		/* Smart tab handling: bug 353610
		 *
		 * If we currently don't have any cells in the space this
		 * tab creates, we try to make the tab character copyable,
		 * by appending a single tab char with lots of fragment
		 * cells following it.
		 *
		 * Otherwise, just append empty cells that will show up
		 * as a space each.
		 */

		old_len = _vte_row_data_length (rowdata);
		_vte_row_data_fill (rowdata, &screen->fill_defaults, newcol);

		/* Insert smart tab if there's nothing in the line after
		 * us.  Though, there may be empty cells (with non-default
		 * background color for example.
		 *
		 * Notable bugs here: 545924 and 597242 */
		{
			glong i;
			gboolean found = FALSE;
			for (i = old_len; i > col; i--) {
				const VteCell *cell = _vte_row_data_get (rowdata, i - 1);
				if (cell->attr.fragment || cell->c != 0) {
					found = TRUE;
					break;
				}
			}
			/* Nothing found on the line after us, turn this into
			 * a smart tab */
			if (!found) {
				VteCell *cell = _vte_row_data_get_writable (rowdata, col);
				VteCell tab = *cell;
				tab.attr.columns = newcol - col;
				tab.c = '\t';
				/* Check if it fits in columns */
				if (tab.attr.columns == newcol - col) {
					/* Save tab char */
					*cell = tab;
					/* And adjust the fragments */
					for (i = col + 1; i < newcol; i++) {
						cell = _vte_row_data_get_writable (rowdata, i);
						cell->c = '\t';
						cell->attr.columns = 1;
						cell->attr.fragment = 1;
					}
				}
			}
		}

		_vte_invalidate_cells (terminal,
				screen->cursor_current.col,
				newcol - screen->cursor_current.col,
				screen->cursor_current.row, 1);
		screen->cursor_current.col = newcol;
	}
}