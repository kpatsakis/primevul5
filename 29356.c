static struct inode *nfs_alloc_inode(struct super_block *sb)
{
	struct nfs_inode *node;

	node = xzalloc(sizeof(*node));
	if (!node)
		return NULL;

	return &node->inode;
}
