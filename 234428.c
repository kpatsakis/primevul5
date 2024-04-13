replot_command()
{
    if (!*replot_line)
	int_error(c_token, "no previous plot");

    if (volatile_data && (refresh_ok != E_REFRESH_NOT_OK) && !replot_disabled) {
	FPRINTF((stderr,"volatile_data %d refresh_ok %d plotted_data_from_stdin %d\n",
		volatile_data, refresh_ok, plotted_data_from_stdin));
	refresh_command();
	return;
    }

    /* Disable replot for some reason; currently used by the mouse/hotkey
       capable terminals to avoid replotting when some data come from stdin,
       i.e. when  plotted_data_from_stdin==1  after plot "-".
    */
    if (replot_disabled) {
	replot_disabled = FALSE;
	bail_to_command_line(); /* be silent --- don't mess the screen */
    }
    if (!term) /* unknown terminal */
	int_error(c_token, "use 'set term' to set terminal type first");

    c_token++;
    SET_CURSOR_WAIT;
    if (term->flags & TERM_INIT_ON_REPLOT)
	term->init();
    replotrequest();
    SET_CURSOR_ARROW;
}