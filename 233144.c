tree_add_word(
    spellinfo_T	*spin,
    char_u	*word,
    wordnode_T	*root,
    int		flags,
    int		region,
    int		affixID)
{
    wordnode_T	*node = root;
    wordnode_T	*np;
    wordnode_T	*copyp, **copyprev;
    wordnode_T	**prev = NULL;
    int		i;

    /* Add each byte of the word to the tree, including the NUL at the end. */
    for (i = 0; ; ++i)
    {
	/* When there is more than one reference to this node we need to make
	 * a copy, so that we can modify it.  Copy the whole list of siblings
	 * (we don't optimize for a partly shared list of siblings). */
	if (node != NULL && node->wn_refs > 1)
	{
	    --node->wn_refs;
	    copyprev = prev;
	    for (copyp = node; copyp != NULL; copyp = copyp->wn_sibling)
	    {
		/* Allocate a new node and copy the info. */
		np = get_wordnode(spin);
		if (np == NULL)
		    return FAIL;
		np->wn_child = copyp->wn_child;
		if (np->wn_child != NULL)
		    ++np->wn_child->wn_refs;	/* child gets extra ref */
		np->wn_byte = copyp->wn_byte;
		if (np->wn_byte == NUL)
		{
		    np->wn_flags = copyp->wn_flags;
		    np->wn_region = copyp->wn_region;
		    np->wn_affixID = copyp->wn_affixID;
		}

		/* Link the new node in the list, there will be one ref. */
		np->wn_refs = 1;
		if (copyprev != NULL)
		    *copyprev = np;
		copyprev = &np->wn_sibling;

		/* Let "node" point to the head of the copied list. */
		if (copyp == node)
		    node = np;
	    }
	}

	/* Look for the sibling that has the same character.  They are sorted
	 * on byte value, thus stop searching when a sibling is found with a
	 * higher byte value.  For zero bytes (end of word) the sorting is
	 * done on flags and then on affixID. */
	while (node != NULL
		&& (node->wn_byte < word[i]
		    || (node->wn_byte == NUL
			&& (flags < 0
			    ? node->wn_affixID < (unsigned)affixID
			    : (node->wn_flags < (unsigned)(flags & WN_MASK)
				|| (node->wn_flags == (flags & WN_MASK)
				    && (spin->si_sugtree
					? (node->wn_region & 0xffff) < region
					: node->wn_affixID
						    < (unsigned)affixID)))))))
	{
	    prev = &node->wn_sibling;
	    node = *prev;
	}
	if (node == NULL
		|| node->wn_byte != word[i]
		|| (word[i] == NUL
		    && (flags < 0
			|| spin->si_sugtree
			|| node->wn_flags != (flags & WN_MASK)
			|| node->wn_affixID != affixID)))
	{
	    /* Allocate a new node. */
	    np = get_wordnode(spin);
	    if (np == NULL)
		return FAIL;
	    np->wn_byte = word[i];

	    /* If "node" is NULL this is a new child or the end of the sibling
	     * list: ref count is one.  Otherwise use ref count of sibling and
	     * make ref count of sibling one (matters when inserting in front
	     * of the list of siblings). */
	    if (node == NULL)
		np->wn_refs = 1;
	    else
	    {
		np->wn_refs = node->wn_refs;
		node->wn_refs = 1;
	    }
	    if (prev != NULL)
		*prev = np;
	    np->wn_sibling = node;
	    node = np;
	}

	if (word[i] == NUL)
	{
	    node->wn_flags = flags;
	    node->wn_region |= region;
	    node->wn_affixID = affixID;
	    break;
	}
	prev = &node->wn_child;
	node = *prev;
    }
#ifdef SPELL_PRINTTREE
    smsg((char_u *)"Added \"%s\"", word);
    spell_print_tree(root->wn_sibling);
#endif

    /* count nr of words added since last message */
    ++spin->si_msg_count;

    if (spin->si_compress_cnt > 1)
    {
	if (--spin->si_compress_cnt == 1)
	    /* Did enough words to lower the block count limit. */
	    spin->si_blocks_cnt += compress_inc;
    }

    /*
     * When we have allocated lots of memory we need to compress the word tree
     * to free up some room.  But compression is slow, and we might actually
     * need that room, thus only compress in the following situations:
     * 1. When not compressed before (si_compress_cnt == 0): when using
     *    "compress_start" blocks.
     * 2. When compressed before and used "compress_inc" blocks before
     *    adding "compress_added" words (si_compress_cnt > 1).
     * 3. When compressed before, added "compress_added" words
     *    (si_compress_cnt == 1) and the number of free nodes drops below the
     *    maximum word length.
     */
#ifndef SPELL_COMPRESS_ALLWAYS
    if (spin->si_compress_cnt == 1
	    ? spin->si_free_count < MAXWLEN
	    : spin->si_blocks_cnt >= compress_start)
#endif
    {
	/* Decrement the block counter.  The effect is that we compress again
	 * when the freed up room has been used and another "compress_inc"
	 * blocks have been allocated.  Unless "compress_added" words have
	 * been added, then the limit is put back again. */
	spin->si_blocks_cnt -= compress_inc;
	spin->si_compress_cnt = compress_added;

	if (spin->si_verbose)
	{
	    msg_start();
	    msg_puts((char_u *)_(msg_compressing));
	    msg_clr_eos();
	    msg_didout = FALSE;
	    msg_col = 0;
	    out_flush();
	}

	/* Compress both trees.  Either they both have many nodes, which makes
	 * compression useful, or one of them is small, which means
	 * compression goes fast.  But when filling the soundfold word tree
	 * there is no keep-case tree. */
	wordtree_compress(spin, spin->si_foldroot);
	if (affixID >= 0)
	    wordtree_compress(spin, spin->si_keeproot);
    }

    return OK;
}