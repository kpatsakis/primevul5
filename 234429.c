is_history_command(const char *line)
{
    int i;
    int start = 0;
    int length = 0;
    int after = 0;
    const char str[] = "hi$story";

    /* skip leading whitespace */
    while (isblank((unsigned char) line[start]))
	start++;

    /* find end of "token" */
    while ((line[start + length] != NUL) && !isblank((unsigned char) line[start + length]))
	length++;

    for (i = 0; i < length + after; i++) {
	if (str[i] != line[start + i]) {
	    if (str[i] != '$')
		return FALSE;
	    else {
		after = 1;
		start--;	/* back up token ptr */
	    }
	}
    }

    /* i now beyond end of token string */

    return (after || str[i] == '$' || str[i] == NUL);
}