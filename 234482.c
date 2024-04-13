term_close_output()
{
    FPRINTF((stderr, "term_close_output\n"));

    opened_binary = FALSE;

    if (!outstr)                /* ie using stdout */
	return;

#if defined(PIPES)
    if (output_pipe_open) {
	(void) pclose(gpoutfile);
	output_pipe_open = FALSE;
    } else
#endif /* PIPES */
#ifdef _WIN32
    if (stricmp(outstr, "PRN") == 0)
	close_printer(gpoutfile);
    else
#endif
    if (gpoutfile != gppsfile)
	fclose(gpoutfile);

    gpoutfile = stdout;         /* Don't dup... */
    free(outstr);
    outstr = NULL;

    if (gppsfile)
	fclose(gppsfile);
    gppsfile = NULL;
}