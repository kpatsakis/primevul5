node_compress(
    spellinfo_T	*spin,
    wordnode_T	*node,
    hashtab_T	*ht,
    int		*tot)	    /* total count of nodes before compressing,
			       incremented while going through the tree */
{
    wordnode_T	*np;
    wordnode_T	*tp;
    wordnode_T	*child;
    hash_T	hash;
    hashitem_T	*hi;
    int		len = 0;
    unsigned	nr, n;
    int		compressed = 0;

    /*
     * Go through the list of siblings.  Compress each child and then try
     * finding an identical child to replace it.
     * Note that with "child" we mean not just the node that is pointed to,
     * but the whole list of siblings of which the child node is the first.
     */
    for (np = node; np != NULL && !got_int; np = np->wn_sibling)
    {
	++len;
	if ((child = np->wn_child) != NULL)
	{
	    /* Compress the child first.  This fills hashkey. */
	    compressed += node_compress(spin, child, ht, tot);

	    /* Try to find an identical child. */
	    hash = hash_hash(child->wn_u1.hashkey);
	    hi = hash_lookup(ht, child->wn_u1.hashkey, hash);
	    if (!HASHITEM_EMPTY(hi))
	    {
		/* There are children we encountered before with a hash value
		 * identical to the current child.  Now check if there is one
		 * that is really identical. */
		for (tp = HI2WN(hi); tp != NULL; tp = tp->wn_u2.next)
		    if (node_equal(child, tp))
		    {
			/* Found one!  Now use that child in place of the
			 * current one.  This means the current child and all
			 * its siblings is unlinked from the tree. */
			++tp->wn_refs;
			compressed += deref_wordnode(spin, child);
			np->wn_child = tp;
			break;
		    }
		if (tp == NULL)
		{
		    /* No other child with this hash value equals the child of
		     * the node, add it to the linked list after the first
		     * item. */
		    tp = HI2WN(hi);
		    child->wn_u2.next = tp->wn_u2.next;
		    tp->wn_u2.next = child;
		}
	    }
	    else
		/* No other child has this hash value, add it to the
		 * hashtable. */
		hash_add_item(ht, hi, child->wn_u1.hashkey, hash);
	}
    }
    *tot += len + 1;	/* add one for the node that stores the length */

    /*
     * Make a hash key for the node and its siblings, so that we can quickly
     * find a lookalike node.  This must be done after compressing the sibling
     * list, otherwise the hash key would become invalid by the compression.
     */
    node->wn_u1.hashkey[0] = len;
    nr = 0;
    for (np = node; np != NULL; np = np->wn_sibling)
    {
	if (np->wn_byte == NUL)
	    /* end node: use wn_flags, wn_region and wn_affixID */
	    n = np->wn_flags + (np->wn_region << 8) + (np->wn_affixID << 16);
	else
	    /* byte node: use the byte value and the child pointer */
	    n = (unsigned)(np->wn_byte + ((long_u)np->wn_child << 8));
	nr = nr * 101 + n;
    }

    /* Avoid NUL bytes, it terminates the hash key. */
    n = nr & 0xff;
    node->wn_u1.hashkey[1] = n == 0 ? 1 : n;
    n = (nr >> 8) & 0xff;
    node->wn_u1.hashkey[2] = n == 0 ? 1 : n;
    n = (nr >> 16) & 0xff;
    node->wn_u1.hashkey[3] = n == 0 ? 1 : n;
    n = (nr >> 24) & 0xff;
    node->wn_u1.hashkey[4] = n == 0 ? 1 : n;
    node->wn_u1.hashkey[5] = NUL;

    /* Check for CTRL-C pressed now and then. */
    fast_breakcheck();

    return compressed;
}