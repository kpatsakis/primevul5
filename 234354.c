extend_input_line()
{
    if (gp_input_line_len == 0) {
	/* first time */
	gp_input_line = gp_alloc(MAX_LINE_LEN, "gp_input_line");
	gp_input_line_len = MAX_LINE_LEN;
	gp_input_line[0] = NUL;

#ifdef OS2_IPC
	sprintf( mouseSharedMemName, "\\SHAREMEM\\GP%i_Mouse_Input", getpid() );
	if (DosAllocSharedMem((PVOID) & input_from_PM_Terminal,
		mouseSharedMemName, MAX_LINE_LEN, PAG_WRITE | PAG_COMMIT))
	    fputs("command.c: DosAllocSharedMem ERROR\n",stderr);
#endif /* OS2_IPC */

    } else {
	gp_input_line = gp_realloc(gp_input_line, gp_input_line_len + MAX_LINE_LEN,
				"extend input line");
	gp_input_line_len += MAX_LINE_LEN;
	FPRINTF((stderr, "extending input line to %d chars\n",
		 gp_input_line_len));
    }
}