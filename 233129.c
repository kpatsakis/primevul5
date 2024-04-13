wordtree_compress(spellinfo_T *spin, wordnode_T *root)
{
    hashtab_T	    ht;
    int		    n;
    int		    tot = 0;
    int		    perc;

    /* Skip the root itself, it's not actually used.  The first sibling is the
     * start of the tree. */
    if (root->wn_sibling != NULL)
    {
	hash_init(&ht);
	n = node_compress(spin, root->wn_sibling, &ht, &tot);

#ifndef SPELL_PRINTTREE
	if (spin->si_verbose || p_verbose > 2)
#endif
	{
	    if (tot > 1000000)
		perc = (tot - n) / (tot / 100);
	    else if (tot == 0)
		perc = 0;
	    else
		perc = (tot - n) * 100 / tot;
	    vim_snprintf((char *)IObuff, IOSIZE,
			  _("Compressed %d of %d nodes; %d (%d%%) remaining"),
						       n, tot, tot - n, perc);
	    spell_message(spin, IObuff);
	}
#ifdef SPELL_PRINTTREE
	spell_print_tree(root->wn_sibling);
#endif
	hash_clear(&ht);
    }
}