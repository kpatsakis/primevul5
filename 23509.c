int vmtruncate(struct inode * inode, loff_t offset)
{
	if (inode->i_size < offset) {
		unsigned long limit;

		limit = current->signal->rlim[RLIMIT_FSIZE].rlim_cur;
		if (limit != RLIM_INFINITY && offset > limit)
			goto out_sig;
		if (offset > inode->i_sb->s_maxbytes)
			goto out_big;
		i_size_write(inode, offset);
	} else {
		struct address_space *mapping = inode->i_mapping;

		/*
		 * truncation of in-use swapfiles is disallowed - it would
		 * cause subsequent swapout to scribble on the now-freed
		 * blocks.
		 */
		if (IS_SWAPFILE(inode))
			return -ETXTBSY;
		i_size_write(inode, offset);

		/*
		 * unmap_mapping_range is called twice, first simply for
		 * efficiency so that truncate_inode_pages does fewer
		 * single-page unmaps.  However after this first call, and
		 * before truncate_inode_pages finishes, it is possible for
		 * private pages to be COWed, which remain after
		 * truncate_inode_pages finishes, hence the second
		 * unmap_mapping_range call must be made for correctness.
		 */
		unmap_mapping_range(mapping, offset + PAGE_SIZE - 1, 0, 1);
		truncate_inode_pages(mapping, offset);
		unmap_mapping_range(mapping, offset + PAGE_SIZE - 1, 0, 1);
	}

	if (inode->i_op && inode->i_op->truncate)
		inode->i_op->truncate(inode);
	return 0;

out_sig:
	send_sig(SIGXFSZ, current, 0);
out_big:
	return -EFBIG;
}