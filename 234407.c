do_system(const char *cmd)
{

     if (!cmd)
	return;

    /* gp_input_line is filled by read_line or load_file, but
     * line_desc length is set only by read_line; adjust now
     */
    line_desc.dsc$w_length = strlen(cmd);
    line_desc.dsc$a_pointer = (char *) cmd;

    if ((vaxc$errno = lib$spawn(&line_desc)) != SS$_NORMAL)
	os_error(NO_CARET, "spawn error");

    (void) putc('\n', stderr);

}