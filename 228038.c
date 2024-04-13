get_buffcont(
    buffheader_T	*buffer,
    int			dozero)	    // count == zero is not an error
{
    long_u	    count = 0;
    char_u	    *p = NULL;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;

    // compute the total length of the string
    for (bp = buffer->bh_first.b_next; bp != NULL; bp = bp->b_next)
	count += (long_u)STRLEN(bp->b_str);

    if ((count || dozero) && (p = alloc(count + 1)) != NULL)
    {
	p2 = p;
	for (bp = buffer->bh_first.b_next; bp != NULL; bp = bp->b_next)
	    for (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
    }
    return (p);
}