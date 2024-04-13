fgets_ipc(
    char *dest,			/* string to fill */
    int len)			/* size of it */
{
#ifdef USE_MOUSE
    if (term && term->waitforinput) {
	/* This a mouseable terminal --- must expect input from it */
	int c;			/* char gotten from waitforinput() */
	size_t i=0;		/* position inside dest */

	dest[0] = '\0';
	for (i=0; i < len-1; i++) {
	    c = term->waitforinput(0);
	    if ('\n' == c) {
		dest[i] = '\n';
		i++;
		break;
	    } else if (EOF == c) {
		dest[i] = '\0';
		return (char*) 0;
	    } else {
		dest[i] = c;
	    }
	}
	dest[i] = '\0';
	return dest;
    } else
#endif
	return fgets(dest, len, stdin);
}