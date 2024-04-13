do_system_func(const char *cmd, char **output)
{

#if defined(VMS) || defined(PIPES)
    int c;
    FILE *f;
    int result_allocated, result_pos;
    char* result;
    int ierr = 0;
# if defined(VMS)
    int chan, one = 1;
    struct dsc$descriptor_s pgmdsc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
    static $DESCRIPTOR(lognamedsc, "PLOT$MAILBOX");
# endif /* VMS */

    /* open stream */
# ifdef VMS
    pgmdsc.dsc$a_pointer = cmd;
    pgmdsc.dsc$w_length = strlen(cmd);
    if (!((vaxc$errno = sys$crembx(0, &chan, 0, 0, 0, 0, &lognamedsc)) & 1))
	os_error(NO_CARET, "sys$crembx failed");

    if (!((vaxc$errno = lib$spawn(&pgmdsc, 0, &lognamedsc, &one)) & 1))
	os_error(NO_CARET, "lib$spawn failed");

    if ((f = fopen("PLOT$MAILBOX", "r")) == NULL)
	os_error(NO_CARET, "mailbox open failed");
# else	/* everyone else */
    restrict_popen();
    if ((f = popen(cmd, "r")) == NULL)
	os_error(NO_CARET, "popen failed");
# endif	/* everyone else */

    /* get output */
    result_pos = 0;
    result_allocated = MAX_LINE_LEN;
    result = gp_alloc(MAX_LINE_LEN, "do_system_func");
    result[0] = NUL;
    while (1) {
	if ((c = getc(f)) == EOF)
	    break;
	/* result <- c */
	result[result_pos++] = c;
	if ( result_pos == result_allocated ) {
	    if ( result_pos >= MAX_TOTAL_LINE_LEN ) {
		result_pos--;
		int_warn(NO_CARET,
			 "*very* long system call output has been truncated");
		break;
	    } else {
		result = gp_realloc(result, result_allocated + MAX_LINE_LEN,
				    "extend in do_system_func");
		result_allocated += MAX_LINE_LEN;
	    }
	}
    }
    result[result_pos] = NUL;

    /* close stream */
    ierr = pclose(f);

    ierr = report_error(ierr);

    result = gp_realloc(result, strlen(result)+1, "do_system_func");
    *output = result;

    return ierr;

#else /* VMS || PIPES */

    int_warn(NO_CARET, "system() requires support for pipes");
    *output = gp_strdup("");
    return 0;

#endif /* VMS || PIPES */

}