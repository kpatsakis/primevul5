static void nfs4_return_incompatible_delegation(struct inode *inode, fmode_t fmode)
{
	struct nfs_delegation *delegation;

	fmode &= FMODE_READ|FMODE_WRITE;
	rcu_read_lock();
	delegation = nfs4_get_valid_delegation(inode);
	if (delegation == NULL || (delegation->type & fmode) == fmode) {
		rcu_read_unlock();
		return;
	}
	rcu_read_unlock();
	nfs4_inode_return_delegation(inode);
}