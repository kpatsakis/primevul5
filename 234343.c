test_command()
{
    int what;
    int save_token = c_token++;

    if (!term) /* unknown terminal */
	int_error(c_token, "use 'set term' to set terminal type first");

    what = lookup_table(&test_tbl[0], c_token);
    switch (what) {
	default:
	    if (!END_OF_COMMAND)
		int_error(c_token, "unrecognized test option");
	    /* otherwise fall through to test_term */
	case TEST_TERMINAL: test_term(); break;
	case TEST_PALETTE: test_palette_subcommand(); break;
    }

    /* prevent annoying error messages if there was no previous plot */
    /* and the "test" window is resized. */
    if (!replot_line || !(*replot_line)) {
	m_capture( &replot_line, save_token, c_token );
    }
}