get_terminals_names()
{
    int i;
    char *buf = gp_alloc(TERMCOUNT*15, "all_term_names"); /* max 15 chars per name */
    char *names;
    int sort_idxs[TERMCOUNT];

    /* sort terminal types alphabetically */
    for( i = 0; i < TERMCOUNT; i++ )
	sort_idxs[i] = i;
    qsort( sort_idxs, TERMCOUNT, sizeof(int), termcomp );
    /* now sort_idxs[] contains the sorted indices */

    strcpy(buf, " "); /* let the string have leading and trailing " " in order to search via strstrt(GPVAL_TERMINALS, " png "); */
    for (i = 0; i < TERMCOUNT; i++)
	sprintf(buf+strlen(buf), "%s ", term_tbl[sort_idxs[i]].name);
    names = gp_alloc(strlen(buf)+1, "all_term_names2");
    strcpy(names, buf);
    free(buf);
    return names;
}