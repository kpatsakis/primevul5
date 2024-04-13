static int list_cb(char *name, int matchlen,
		   int maycreate __attribute__((unused)), void *rock)
{
    static char lastname[MAX_MAILBOX_BUFFER];
    struct list_rock *lrock = (struct list_rock *) rock;
    struct wildmat *wild;

    /* We have to reset the initial state.
     * Handle it as a dirty hack.
     */
    if (!name) {
	lastname[0] = '\0';
	return 0;
    }

    /* skip mailboxes that we don't want to serve as newsgroups */
    if (!is_newsgroup(name)) return 0;

    /* don't repeat */
    if (matchlen == (int) strlen(lastname) &&
	!strncmp(name, lastname, matchlen)) return 0;

    strncpy(lastname, name, matchlen);
    lastname[matchlen] = '\0';

    /* see if the mailbox matches one of our specified wildmats */
    wild = lrock->wild;
    while (wild->pat && wildmat(name, wild->pat) != 1) wild++;

    /* if we don't have a match, or its a negative match, skip it */
    if (!wild->pat || wild->not) return 0;

    return lrock->proc(name, lrock);
}