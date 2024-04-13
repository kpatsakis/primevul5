clear_node(wordnode_T *node)
{
    wordnode_T	*np;

    if (node != NULL)
	for (np = node; np != NULL; np = np->wn_sibling)
	{
	    np->wn_u1.index = 0;
	    np->wn_u2.wnode = NULL;

	    if (np->wn_byte != NUL)
		clear_node(np->wn_child);
	}
}