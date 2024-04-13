read_words_section(FILE *fd, slang_T *lp, int len)
{
    int		done = 0;
    int		i;
    int		c;
    char_u	word[MAXWLEN];

    while (done < len)
    {
	/* Read one word at a time. */
	for (i = 0; ; ++i)
	{
	    c = getc(fd);
	    if (c == EOF)
		return SP_TRUNCERROR;
	    word[i] = c;
	    if (word[i] == NUL)
		break;
	    if (i == MAXWLEN - 1)
		return SP_FORMERROR;
	}

	/* Init the count to 10. */
	count_common_word(lp, word, -1, 10);
	done += i + 1;
    }
    return 0;
}