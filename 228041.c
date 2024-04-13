flush_buffers(flush_buffers_T flush_typeahead)
{
    init_typebuf();

    start_stuff();
    while (read_readbuffers(TRUE) != NUL)
	;

    if (flush_typeahead == FLUSH_MINIMAL)
    {
	// remove mapped characters at the start only
	typebuf.tb_off += typebuf.tb_maplen;
	typebuf.tb_len -= typebuf.tb_maplen;
#if defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	if (typebuf.tb_len == 0)
	    typebuf_was_filled = FALSE;
#endif
    }
    else
    {
	// remove typeahead
	if (flush_typeahead == FLUSH_INPUT)
	    // We have to get all characters, because we may delete the first
	    // part of an escape sequence.  In an xterm we get one char at a
	    // time and we have to get them all.
	    while (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L) != 0)
		;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
#if defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	// Reset the flag that text received from a client or from feedkeys()
	// was inserted in the typeahead buffer.
	typebuf_was_filled = FALSE;
#endif
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
    if (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}