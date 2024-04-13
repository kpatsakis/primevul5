com_line()
{
#ifdef OS2_IPC
static char *input_line_SharedMem = NULL;

    if (input_line_SharedMem == NULL) {  /* get shared mem only once */
    if (DosGetNamedSharedMem((PVOID) &input_line_SharedMem,
		mouseSharedMemName, PAG_WRITE | PAG_READ))
	fputs("readline.c: DosGetNamedSharedMem ERROR\n", stderr);
    else
	*input_line_SharedMem = 0;
    }
#endif /* OS2_IPC */

    if (multiplot) {
	/* calls int_error() if it is not happy */
	term_check_multiplot_okay(interactive);

	if (read_line("multiplot> ", 0))
	    return (1);
    } else {

#if defined(OS2_IPC) && defined(USE_MOUSE)
	ULONG u;
	if (thread_rl_Running == 0) {
	    int res = _beginthread(thread_read_line,NULL,32768,NULL);
	    if (res == -1)
		fputs("error command.c could not begin thread\n",stderr);
	}
	/* wait until a line is read or gnupmdrv makes shared mem available */
	DosWaitEventSem(semInputReady,SEM_INDEFINITE_WAIT);
	DosResetEventSem(semInputReady,&u);
	if (thread_rl_Running) {
	    if (input_line_SharedMem == NULL || !*input_line_SharedMem)
		return (0);
	    if (*input_line_SharedMem=='%') {
		do_event( (struct gp_event_t*)(input_line_SharedMem+1) ); /* pass terminal's event */
		input_line_SharedMem[0] = 0; /* discard the whole command line */
		thread_rl_RetCode = 0;
		return (0);
	    }
	    if (*input_line_SharedMem &&
		strstr(input_line_SharedMem,"plot") != NULL &&
		(strcmp(term->name,"pm") && strcmp(term->name,"x11"))) {
		/* avoid plotting if terminal is not PM or X11 */
		fprintf(stderr,"\n\tCommand(s) ignored for other than PM and X11 terminals\a\n");
		if (interactive) fputs(PROMPT,stderr);
		input_line_SharedMem[0] = 0; /* discard the whole command line */
		return (0);
	    }
	    strcpy(gp_input_line, input_line_SharedMem);
	    input_line_SharedMem[0] = 0;
	    thread_rl_RetCode = 0;
	}
	if (thread_rl_RetCode)
	    return (1);

#else	/* The normal case */
	if (read_line(PROMPT, 0))
	    return (1);
#endif	/* defined(OS2_IPC) && defined(USE_MOUSE) */
    }

    /* So we can flag any new output: if false at time of error,
     * we reprint the command line before printing caret.
     * TRUE for interactive terminals, since the command line is typed.
     * FALSE for non-terminal stdin, so command line is printed anyway.
     * (DFK 11/89)
     */
    screen_ok = interactive;

    if (do_line())
	return (1);
    else
	return (0);
}