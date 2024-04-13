spell_clear_flags(wordnode_T *node)
{
    wordnode_T	*np;

    for (np = node; np != NULL; np = np->wn_sibling)
    {
	np->wn_u1.index = FALSE;
	spell_clear_flags(np->wn_child);
    }
}