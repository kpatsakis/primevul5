term_set_output(char *dest)
{
    FILE *f = NULL;

    FPRINTF((stderr, "term_set_output\n"));
    assert(dest == NULL || dest != outstr);

    if (multiplot) {
	fputs("In multiplot mode you can't change the output\n", stderr);
	return;
    }
    if (term && term_initialised) {
	(*term->reset) ();
	term_initialised = FALSE;
	/* switch off output to special postscript file (if used) */
	gppsfile = NULL;
    }
    if (dest == NULL) {         /* stdout */
	term_close_output();
    } else {

#if defined(PIPES)
	if (*dest == '|') {
	    restrict_popen();
#if defined(_WIN32 ) || defined(MSDOS)
	    if (term && (term->flags & TERM_BINARY))
		f = popen(dest + 1, "wb");
	    else
		f = popen(dest + 1, "w");
#else
	    f = popen(dest + 1, "w");
#endif
	    if (f == (FILE *) NULL)
		os_error(c_token, "cannot create pipe; output not changed");
	    else
		output_pipe_open = TRUE;
	} else {
#endif /* PIPES */

#ifdef _WIN32
	if (outstr && stricmp(outstr, "PRN") == 0) {
	    /* we can't call open_printer() while printer is open, so */
	    close_printer(gpoutfile);   /* close printer immediately if open */
	    gpoutfile = stdout; /* and reset output to stdout */
	    free(outstr);
	    outstr = NULL;
	}
	if (stricmp(dest, "PRN") == 0) {
	    if ((f = open_printer()) == (FILE *) NULL)
		os_error(c_token, "cannot open printer temporary file; output may have changed");
	} else
#endif

	{
#if defined (MSDOS)
	    if (outstr && (0 == stricmp(outstr, dest))) {
		/* On MSDOS, you cannot open the same file twice and
		 * then close the first-opened one and keep the second
		 * open, it seems. If you do, you get lost clusters
		 * (connection to the first version of the file is
		 * lost, it seems). */
		/* FIXME: this is not yet safe enough. You can fool it by
		 * specifying the same output file in two different ways
		 * (relative vs. absolute path to file, e.g.) */
		term_close_output();
	    }
#endif
	    if (term && (term->flags & TERM_BINARY))
		f = FOPEN_BINARY(dest);
	    else
		f = fopen(dest, "w");

	    if (f == (FILE *) NULL)
		os_error(c_token, "cannot open file; output not changed");
	}
#if defined(PIPES)
	}
#endif

	term_close_output();
	gpoutfile = f;
	outstr = dest;
	opened_binary = (term && (term->flags & TERM_BINARY));
    }
}