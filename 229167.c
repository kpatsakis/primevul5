add_range(fz_context *ctx, pdf_cmap *cmap, unsigned int low, unsigned int high, unsigned int out, int check_for_overlap, int many)
{
	int current;
	cmap_splay *tree;

	if (low > high)
	{
		fz_warn(ctx, "range limits out of range in cmap %s", cmap->cmap_name);
		return;
	}

	tree = cmap->tree;

	if (cmap->tlen)
	{
		unsigned int move = cmap->ttop;
		unsigned int gt = EMPTY;
		unsigned int lt = EMPTY;
		if (check_for_overlap)
		{
			/* Check for collision with the current node */
			do
			{
				current = move;
				/* Cases we might meet:
				 * tree[i]:        <----->
				 * case 0:     <->
				 * case 1:     <------->
				 * case 2:     <------------->
				 * case 3:           <->
				 * case 4:           <------->
				 * case 5:                 <->
				 */
				if (low <= tree[current].low && tree[current].low <= high)
				{
					/* case 1, reduces to case 0 */
					/* or case 2, deleting the node */
					tree[current].out += high + 1 - tree[current].low;
					tree[current].low = high + 1;
					if (tree[current].low > tree[current].high)
					{
						move = delete_node(cmap, current);
						current = EMPTY;
						continue;
					}
				}
				else if (low <= tree[current].high && tree[current].high <= high)
				{
					/* case 4, reduces to case 5 */
					tree[current].high = low - 1;
					assert(tree[current].low <= tree[current].high);
				}
				else if (tree[current].low < low && high < tree[current].high)
				{
					/* case 3, reduces to case 5 */
					int new_high = tree[current].high;
					tree[current].high = low-1;
					add_range(ctx, cmap, high+1, new_high, tree[current].out + high + 1 - tree[current].low, 0, tree[current].many);
					tree = cmap->tree;
				}
				/* Now look for where to move to next (left for case 0, right for case 5) */
				if (tree[current].low > high) {
					move = tree[current].left;
					gt = current;
				}
				else
				{
					move = tree[current].right;
					lt = current;
				}
			}
			while (move != EMPTY);
		}
		else
		{
			do
			{
				current = move;
				if (tree[current].low > high)
				{
					move = tree[current].left;
					gt = current;
				}
				else
				{
					move = tree[current].right;
					lt = current;
				}
			} while (move != EMPTY);
		}
		/* current is now the node to which we would be adding the new node */
		/* lt is the last node we traversed which is lt the new node. */
		/* gt is the last node we traversed which is gt the new node. */

		if (!many)
		{
			/* Check for the 'merge' cases. */
			if (lt != EMPTY && !tree[lt].many && tree[lt].high == low-1 && tree[lt].out - tree[lt].low == out - low)
			{
				tree[lt].high = high;
				if (gt != EMPTY && !tree[gt].many && tree[gt].low == high+1 && tree[gt].out - tree[gt].low == out - low)
				{
					tree[lt].high = tree[gt].high;
					delete_node(cmap, gt);
				}
				goto exit;
			}
			if (gt != EMPTY && !tree[gt].many && tree[gt].low == high+1 && tree[gt].out - tree[gt].low == out - low)
			{
				tree[gt].low = low;
				tree[gt].out = out;
				goto exit;
			}
		}
	}
	else
		current = EMPTY;

	if (cmap->tlen == cmap->tcap)
	{
		int new_cap = cmap->tcap ? cmap->tcap * 2 : 256;
		tree = cmap->tree = fz_resize_array(ctx, cmap->tree, new_cap, sizeof *cmap->tree);
		cmap->tcap = new_cap;
	}
	tree[cmap->tlen].low = low;
	tree[cmap->tlen].high = high;
	tree[cmap->tlen].out = out;
	tree[cmap->tlen].parent = current;
	tree[cmap->tlen].left = EMPTY;
	tree[cmap->tlen].right = EMPTY;
	tree[cmap->tlen].many = many;
	cmap->tlen++;
	if (current == EMPTY)
		cmap->ttop = 0;
	else if (tree[current].low > high)
		tree[current].left = cmap->tlen-1;
	else
	{
		assert(tree[current].high < low);
		tree[current].right = cmap->tlen-1;
	}
	move_to_root(tree, cmap->tlen-1);
	cmap->ttop = cmap->tlen-1;
exit:
	{}
#ifdef CHECK_SPLAY
	check_splay(cmap->tree, cmap->ttop, 0);
#endif
#ifdef DUMP_SPLAY
	dump_splay(cmap->tree, cmap->ttop, 0, "");
#endif
}