static ssize_t fuse_file_aio_read(struct kiocb *iocb, const struct iovec *iov,
				  unsigned long nr_segs, loff_t pos)
{
	struct inode *inode = iocb->ki_filp->f_mapping->host;

	if (pos + iov_length(iov, nr_segs) > i_size_read(inode)) {
		int err;
		/*
		 * If trying to read past EOF, make sure the i_size
		 * attribute is up-to-date.
		 */
		err = fuse_update_attributes(inode, NULL, iocb->ki_filp, NULL);
		if (err)
			return err;
	}

	return generic_file_aio_read(iocb, iov, nr_segs, pos);
}