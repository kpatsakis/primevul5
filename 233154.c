get_wordnode(spellinfo_T *spin)
{
    wordnode_T *n;

    if (spin->si_first_free == NULL)
	n = (wordnode_T *)getroom(spin, sizeof(wordnode_T), TRUE);
    else
    {
	n = spin->si_first_free;
	spin->si_first_free = n->wn_child;
	vim_memset(n, 0, sizeof(wordnode_T));
	--spin->si_free_count;
    }
#ifdef SPELL_PRINTTREE
    if (n != NULL)
	n->wn_nr = ++spin->si_wordnode_nr;
#endif
    return n;
}