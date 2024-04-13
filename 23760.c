vte_sequence_handler_ic (VteTerminal *terminal, GValueArray *params)
{
	VteVisualPosition save;
	VteScreen *screen;

	screen = terminal->pvt->screen;

	save = screen->cursor_current;

	_vte_terminal_insert_char(terminal, ' ', TRUE, TRUE);

	screen->cursor_current = save;
}