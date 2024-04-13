ssize_t fuse_direct_io(struct fuse_io_priv *io, struct iov_iter *iter,
		       loff_t *ppos, int flags)
{
	int write = flags & FUSE_DIO_WRITE;
	int cuse = flags & FUSE_DIO_CUSE;
	struct file *file = io->iocb->ki_filp;
	struct inode *inode = file->f_mapping->host;
	struct fuse_file *ff = file->private_data;
	struct fuse_conn *fc = ff->fm->fc;
	size_t nmax = write ? fc->max_write : fc->max_read;
	loff_t pos = *ppos;
	size_t count = iov_iter_count(iter);
	pgoff_t idx_from = pos >> PAGE_SHIFT;
	pgoff_t idx_to = (pos + count - 1) >> PAGE_SHIFT;
	ssize_t res = 0;
	int err = 0;
	struct fuse_io_args *ia;
	unsigned int max_pages;

	max_pages = iov_iter_npages(iter, fc->max_pages);
	ia = fuse_io_alloc(io, max_pages);
	if (!ia)
		return -ENOMEM;

	ia->io = io;
	if (!cuse && fuse_range_is_writeback(inode, idx_from, idx_to)) {
		if (!write)
			inode_lock(inode);
		fuse_sync_writes(inode);
		if (!write)
			inode_unlock(inode);
	}

	io->should_dirty = !write && iter_is_iovec(iter);
	while (count) {
		ssize_t nres;
		fl_owner_t owner = current->files;
		size_t nbytes = min(count, nmax);

		err = fuse_get_user_pages(&ia->ap, iter, &nbytes, write,
					  max_pages);
		if (err && !nbytes)
			break;

		if (write) {
			if (!capable(CAP_FSETID))
				ia->write.in.write_flags |= FUSE_WRITE_KILL_SUIDGID;

			nres = fuse_send_write(ia, pos, nbytes, owner);
		} else {
			nres = fuse_send_read(ia, pos, nbytes, owner);
		}

		if (!io->async || nres < 0) {
			fuse_release_user_pages(&ia->ap, io->should_dirty);
			fuse_io_free(ia);
		}
		ia = NULL;
		if (nres < 0) {
			iov_iter_revert(iter, nbytes);
			err = nres;
			break;
		}
		WARN_ON(nres > nbytes);

		count -= nres;
		res += nres;
		pos += nres;
		if (nres != nbytes) {
			iov_iter_revert(iter, nbytes - nres);
			break;
		}
		if (count) {
			max_pages = iov_iter_npages(iter, fc->max_pages);
			ia = fuse_io_alloc(io, max_pages);
			if (!ia)
				break;
		}
	}
	if (ia)
		fuse_io_free(ia);
	if (res > 0)
		*ppos = pos;

	return res > 0 ? res : err;
}