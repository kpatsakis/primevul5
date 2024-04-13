add_buff(
    buffheader_T	*buf,
    char_u		*s,
    long		slen)	// length of "s" or -1
{
    buffblock_T *p;
    long_u	    len;

    if (slen < 0)
	slen = (long)STRLEN(s);
    if (slen == 0)				// don't add empty strings
	return;

    if (buf->bh_first.b_next == NULL)	// first add to list
    {
	buf->bh_space = 0;
	buf->bh_curr = &(buf->bh_first);
    }
    else if (buf->bh_curr == NULL)	// buffer has already been read
    {
	iemsg(_(e_add_to_internal_buffer_that_was_already_read_from));
	return;
    }
    else if (buf->bh_index != 0)
	mch_memmove(buf->bh_first.b_next->b_str,
		    buf->bh_first.b_next->b_str + buf->bh_index,
		    STRLEN(buf->bh_first.b_next->b_str + buf->bh_index) + 1);
    buf->bh_index = 0;

    if (buf->bh_space >= (int)slen)
    {
	len = (long_u)STRLEN(buf->bh_curr->b_str);
	vim_strncpy(buf->bh_curr->b_str + len, s, (size_t)slen);
	buf->bh_space -= slen;
    }
    else
    {
	if (slen < MINIMAL_SIZE)
	    len = MINIMAL_SIZE;
	else
	    len = slen;
	p = alloc(offsetof(buffblock_T, b_str) + len + 1);
	if (p == NULL)
	    return; // no space, just forget it
	buf->bh_space = (int)(len - slen);
	vim_strncpy(p->b_str, s, (size_t)slen);

	p->b_next = buf->bh_curr->b_next;
	buf->bh_curr->b_next = p;
	buf->bh_curr = p;
    }
}