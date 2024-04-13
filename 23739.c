_vte_terminal_scroll_text (VteTerminal *terminal, int scroll_amount)
{
	long start, end, i;
	VteScreen *screen;

	screen = terminal->pvt->screen;

	if (screen->scrolling_restricted) {
		start = screen->insert_delta + screen->scrolling_region.start;
		end = screen->insert_delta + screen->scrolling_region.end;
	} else {
		start = screen->insert_delta;
		end = start + terminal->row_count - 1;
	}

	while (_vte_ring_next(screen->row_data) <= end)
		_vte_terminal_ring_append (terminal, FALSE);

	if (scroll_amount > 0) {
		for (i = 0; i < scroll_amount; i++) {
			_vte_terminal_ring_remove (terminal, end);
			_vte_terminal_ring_insert (terminal, start, TRUE);
		}
	} else {
		for (i = 0; i < -scroll_amount; i++) {
			_vte_terminal_ring_remove (terminal, start);
			_vte_terminal_ring_insert (terminal, end, TRUE);
		}
	}

	/* Update the display. */
	_vte_terminal_scroll_region(terminal, start, end - start + 1,
				   scroll_amount);

	/* Adjust the scrollbars if necessary. */
	_vte_terminal_adjust_adjustments(terminal);

	/* We've modified the display.  Make a note of it. */
	terminal->pvt->text_inserted_flag = TRUE;
	terminal->pvt->text_deleted_flag = TRUE;
}