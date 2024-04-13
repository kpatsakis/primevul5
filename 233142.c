deref_wordnode(spellinfo_T *spin, wordnode_T *node)
{
    wordnode_T	*np;
    int		cnt = 0;

    if (--node->wn_refs == 0)
    {
	for (np = node; np != NULL; np = np->wn_sibling)
	{
	    if (np->wn_child != NULL)
		cnt += deref_wordnode(spin, np->wn_child);
	    free_wordnode(spin, np);
	    ++cnt;
	}
	++cnt;	    /* length field */
    }
    return cnt;
}