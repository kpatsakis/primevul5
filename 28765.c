do_check(cmap_splay *node, void *arg)
{
	cmap_splay *tree = arg;
	unsigned int num = node - tree;
	assert(!node->many || node->low == node->high);
	assert(node->low <= node->high);
	assert((node->left == EMPTY) || (tree[node->left].parent == num &&
		tree[node->left].high < node->low));
	assert(node->right == EMPTY || (tree[node->right].parent == num &&
		node->high < tree[node->right].low));
}
