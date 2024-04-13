list_terms()
{
    int i;
    char *line_buffer = gp_alloc(BUFSIZ, "list_terms");
    int sort_idxs[TERMCOUNT];

    /* sort terminal types alphabetically */
    for( i = 0; i < TERMCOUNT; i++ )
	sort_idxs[i] = i;
    qsort( sort_idxs, TERMCOUNT, sizeof(int), termcomp );
    /* now sort_idxs[] contains the sorted indices */

    StartOutput();
    strcpy(line_buffer, "\nAvailable terminal types:\n");
    OutLine(line_buffer);

    for (i = 0; i < TERMCOUNT; i++) {
	sprintf(line_buffer, "  %15s  %s\n",
		term_tbl[sort_idxs[i]].name,
		term_tbl[sort_idxs[i]].description);
	OutLine(line_buffer);
    }

    EndOutput();
    free(line_buffer);
}