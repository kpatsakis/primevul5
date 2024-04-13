tree_count_words(char_u *byts, idx_T *idxs)
{
    int		depth;
    idx_T	arridx[MAXWLEN];
    int		curi[MAXWLEN];
    int		c;
    idx_T	n;
    int		wordcount[MAXWLEN];

    arridx[0] = 0;
    curi[0] = 1;
    wordcount[0] = 0;
    depth = 0;
    while (depth >= 0 && !got_int)
    {
	if (curi[depth] > byts[arridx[depth]])
	{
	    /* Done all bytes at this node, go up one level. */
	    idxs[arridx[depth]] = wordcount[depth];
	    if (depth > 0)
		wordcount[depth - 1] += wordcount[depth];

	    --depth;
	    fast_breakcheck();
	}
	else
	{
	    /* Do one more byte at this node. */
	    n = arridx[depth] + curi[depth];
	    ++curi[depth];

	    c = byts[n];
	    if (c == 0)
	    {
		/* End of word, count it. */
		++wordcount[depth];

		/* Skip over any other NUL bytes (same word with different
		 * flags). */
		while (byts[n + 1] == 0)
		{
		    ++n;
		    ++curi[depth];
		}
	    }
	    else
	    {
		/* Normal char, go one level deeper to count the words. */
		++depth;
		arridx[depth] = idxs[n];
		curi[depth] = 1;
		wordcount[depth] = 0;
	    }
	}
    }
}