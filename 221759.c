static void fuse_dentry_settime(struct dentry *dentry, u64 time)
{
	struct fuse_conn *fc = get_fuse_conn_super(dentry->d_sb);
	bool delete = !time && fc->delete_stale;
	/*
	 * Mess with DCACHE_OP_DELETE because dput() will be faster without it.
	 * Don't care about races, either way it's just an optimization
	 */
	if ((!delete && (dentry->d_flags & DCACHE_OP_DELETE)) ||
	    (delete && !(dentry->d_flags & DCACHE_OP_DELETE))) {
		spin_lock(&dentry->d_lock);
		if (!delete)
			dentry->d_flags &= ~DCACHE_OP_DELETE;
		else
			dentry->d_flags |= DCACHE_OP_DELETE;
		spin_unlock(&dentry->d_lock);
	}

	__fuse_dentry_settime(dentry, time);
}