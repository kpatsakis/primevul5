read_readbuf(buffheader_T *buf, int advance)
{
    char_u	c;
    buffblock_T	*curr;

    if (buf->bh_first.b_next == NULL)  // buffer is empty
	return NUL;

    curr = buf->bh_first.b_next;
    c = curr->b_str[buf->bh_index];

    if (advance)
    {
	if (curr->b_str[++buf->bh_index] == NUL)
	{
	    buf->bh_first.b_next = curr->b_next;
	    vim_free(curr);
	    buf->bh_index = 0;
	}
    }
    return c;
}