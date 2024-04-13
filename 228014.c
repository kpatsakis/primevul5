start_redo_ins(void)
{
    int	    c;

    if (read_redo(TRUE, FALSE) == FAIL)
	return FAIL;
    start_stuff();

    // skip the count and the command character
    while ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	if (vim_strchr((char_u *)"AaIiRrOo", c) != NULL)
	{
	    if (c == 'O' || c == 'o')
		add_buff(&readbuf2, NL_STR, -1L);
	    break;
	}
    }

    // copy the typed text from the redo buffer into the stuff buffer
    copy_redo(FALSE);
    block_redo = TRUE;
    return OK;
}