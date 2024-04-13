term_initialise()
{
    FPRINTF((stderr, "term_initialise()\n"));

    if (!term)
	int_error(NO_CARET, "No terminal defined");

    /* check if we have opened the output file in the wrong mode
     * (text/binary), if set term comes after set output
     * This was originally done in change_term, but that
     * resulted in output files being truncated
     */

    if (outstr && (term->flags & TERM_NO_OUTPUTFILE)) {
	if (interactive)
	    fprintf(stderr,"Closing %s\n",outstr);
	term_close_output();
    }

    if (outstr &&
	(((term->flags & TERM_BINARY) && !opened_binary) ||
	 ((!(term->flags & TERM_BINARY) && opened_binary)))) {
	/* this is nasty - we cannot just term_set_output(outstr)
	 * since term_set_output will first free outstr and we
	 * end up with an invalid pointer. I think I would
	 * prefer to defer opening output file until first plot.
	 */
	char *temp = gp_alloc(strlen(outstr) + 1, "temp file string");
	if (temp) {
	    FPRINTF((stderr, "term_initialise: reopening \"%s\" as %s\n",
		     outstr, term->flags & TERM_BINARY ? "binary" : "text"));
	    strcpy(temp, outstr);
	    term_set_output(temp);      /* will free outstr */
	    if (temp != outstr) {
		if (temp)
		    free(temp);
		temp = outstr;
	    }
	} else
	    fputs("Cannot reopen output file in binary", stderr);
	/* and carry on, hoping for the best ! */
    }
#if defined(MSDOS) || defined (_WIN32) || defined(OS2)
# ifdef _WIN32
    else if (!outstr && (term->flags & TERM_BINARY))
# else
    else if (!outstr && !interactive && (term->flags & TERM_BINARY))
# endif
	{
#if defined(_WIN32) && !defined(WGP_CONSOLE)
#ifdef PIPES
	    if (!output_pipe_open)
#endif
		if (outstr == NULL && !(term->flags & TERM_NO_OUTPUTFILE))
		    int_error(c_token, "cannot output binary data to wgnuplot text window");
#endif
	    /* binary to stdout in non-interactive session... */
	    fflush(stdout);
	    setmode(fileno(stdout), O_BINARY);
	}
#endif

    if (!term_initialised || term_force_init) {
	FPRINTF((stderr, "- calling term->init()\n"));
	(*term->init) ();
	term_initialised = TRUE;
#ifdef HAVE_LOCALE_H
	/* This is here only from an abundance of caution (a.k.a. paranoia).
	 * Some terminals (wxt qt caca) are known to change the locale when
	 * initialized.  Others have been implicated (gd).  Rather than trying
	 * to catch all such offenders one by one, cover for all of them here.
	 */
	setlocale(LC_NUMERIC, "C");
#endif
    }
}