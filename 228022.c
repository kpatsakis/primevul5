del_typebuf(int len, int offset)
{
    int	    i;

    if (len == 0)
	return;		// nothing to do

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    if (offset == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    else
    {
	i = typebuf.tb_off + offset;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	if (typebuf.tb_off > MAXMAPLEN)
	{
	    mch_memmove(typebuf.tb_buf + MAXMAPLEN,
			     typebuf.tb_buf + typebuf.tb_off, (size_t)offset);
	    mch_memmove(typebuf.tb_noremap + MAXMAPLEN,
			 typebuf.tb_noremap + typebuf.tb_off, (size_t)offset);
	    typebuf.tb_off = MAXMAPLEN;
	}
	// adjust typebuf.tb_buf (include the NUL at the end)
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + offset,
						     typebuf.tb_buf + i + len,
				       (size_t)(typebuf.tb_len - offset + 1));
	// adjust typebuf.tb_noremap[]
	mch_memmove(typebuf.tb_noremap + typebuf.tb_off + offset,
						 typebuf.tb_noremap + i + len,
					   (size_t)(typebuf.tb_len - offset));
    }

    if (typebuf.tb_maplen > offset)		// adjust tb_maplen
    {
	if (typebuf.tb_maplen < offset + len)
	    typebuf.tb_maplen = offset;
	else
	    typebuf.tb_maplen -= len;
    }
    if (typebuf.tb_silent > offset)		// adjust tb_silent
    {
	if (typebuf.tb_silent < offset + len)
	    typebuf.tb_silent = offset;
	else
	    typebuf.tb_silent -= len;
    }
    if (typebuf.tb_no_abbr_cnt > offset)	// adjust tb_no_abbr_cnt
    {
	if (typebuf.tb_no_abbr_cnt < offset + len)
	    typebuf.tb_no_abbr_cnt = offset;
	else
	    typebuf.tb_no_abbr_cnt -= len;
    }

#if defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // Reset the flag that text received from a client or from feedkeys()
    // was inserted in the typeahead buffer.
    typebuf_was_filled = FALSE;
#endif
    if (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}