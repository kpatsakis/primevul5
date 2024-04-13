nfs4_wake_lock_waiter(wait_queue_entry_t *wait, unsigned int mode, int flags, void *key)
{
	int ret;
	struct nfs4_lock_waiter	*waiter	= wait->private;

	/* NULL key means to wake up everyone */
	if (key) {
		struct cb_notify_lock_args	*cbnl = key;
		struct nfs_lowner		*lowner = &cbnl->cbnl_owner,
						*wowner = waiter->owner;

		/* Only wake if the callback was for the same owner. */
		if (lowner->id != wowner->id || lowner->s_dev != wowner->s_dev)
			return 0;

		/* Make sure it's for the right inode */
		if (nfs_compare_fh(NFS_FH(waiter->inode), &cbnl->cbnl_fh))
			return 0;
	}

	/* override "private" so we can use default_wake_function */
	wait->private = waiter->task;
	ret = woken_wake_function(wait, mode, flags, key);
	if (ret)
		list_del_init(&wait->entry);
	wait->private = waiter;
	return ret;
}