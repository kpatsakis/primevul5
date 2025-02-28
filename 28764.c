static unsigned int delete_node(pdf_cmap *cmap, unsigned int current)
{
	cmap_splay *tree = cmap->tree;
	unsigned int parent;
	unsigned int replacement;

	assert(current != EMPTY);

	parent = tree[current].parent;
	if (tree[current].right == EMPTY)
	{
		if (parent == EMPTY)
		{
			replacement = cmap->ttop = tree[current].left;
		}
		else if (tree[parent].left == current)
		{
			replacement = tree[parent].left = tree[current].left;
		}
		else
		{
			assert(tree[parent].right == current);
			replacement = tree[parent].right = tree[current].left;
		}
		if (replacement != EMPTY)
			tree[replacement].parent = parent;
		else
			replacement = parent;
	}
	else if (tree[current].left == EMPTY)
	{
		if (parent == EMPTY)
		{
			replacement = cmap->ttop = tree[current].right;
		}
		else if (tree[parent].left == current)
		{
			replacement = tree[parent].left = tree[current].right;
		}
		else
		{
			assert(tree[parent].right == current);
			replacement = tree[parent].right = tree[current].right;
		}
		if (replacement != EMPTY)
			tree[replacement].parent = parent;
		else
			replacement = parent;
	}
	else
	{
		/* Hard case, find the in-order predecessor of current */
		int amputee = current;
		replacement = tree[current].left;
		while (tree[replacement].right != EMPTY) {
			amputee = replacement;
			replacement = tree[replacement].right;
		}
		/* Remove replacement from the tree */
		if (amputee == current)
		{
			tree[amputee].left = tree[replacement].left;
			if (tree[amputee].left != EMPTY)
				tree[tree[amputee].left].parent = amputee;
		}
		else
		{
			tree[amputee].right = tree[replacement].left;
			if (tree[amputee].right != EMPTY)
				tree[tree[amputee].right].parent = amputee;
		}
		/* Insert replacement in place of current */
		tree[replacement].parent = parent;
		if (parent == EMPTY)
		{
			tree[replacement].parent = EMPTY;
			cmap->ttop = replacement;
		}
		else if (tree[parent].left == current)
			tree[parent].left = replacement;
		else
		{
			assert(tree[parent].right == current);
			tree[parent].right = replacement;
		}
		tree[replacement].left = tree[current].left;
		if (tree[replacement].left != EMPTY)
			tree[tree[replacement].left].parent = replacement;
		tree[replacement].right = tree[current].right;
		if (tree[replacement].right != EMPTY)
			tree[tree[replacement].right].parent = replacement;
	}

	/* current is now unlinked. We need to remove it from our array. */
	cmap->tlen--;
	if (current != cmap->tlen)
	{
		if (replacement == cmap->tlen)
			replacement = current;
		tree[current] = tree[cmap->tlen];
		parent = tree[current].parent;
		if (parent == EMPTY)
			cmap->ttop = current;
		else if (tree[parent].left == cmap->tlen)
			tree[parent].left = current;
		else
		{
			assert(tree[parent].right == cmap->tlen);
			tree[parent].right = current;
		}
		if (tree[current].left != EMPTY)
		{
			assert(tree[tree[current].left].parent == cmap->tlen);
			tree[tree[current].left].parent = current;
		}
		if (tree[current].right != EMPTY)
		{
			assert(tree[tree[current].right].parent == cmap->tlen);
			tree[tree[current].right].parent = current;
		}
	}

	/* Return the node that we should continue searching from */
	return replacement;
}
