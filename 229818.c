nfs4_wait_on_layoutreturn(struct inode *inode, struct rpc_task *task)
{
	if (inode == NULL || !nfs_have_layout(inode))
		return false;

	return pnfs_wait_on_layoutreturn(inode, task);
}