static ssize_t fuse_direct_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	struct fuse_io_priv io = FUSE_IO_PRIV_SYNC(iocb);
	ssize_t res;

	/* Don't allow parallel writes to the same file */
	inode_lock(inode);
	res = generic_write_checks(iocb, from);
	if (res > 0) {
		if (!is_sync_kiocb(iocb) && iocb->ki_flags & IOCB_DIRECT) {
			res = fuse_direct_IO(iocb, from);
		} else {
			res = fuse_direct_io(&io, from, &iocb->ki_pos,
					     FUSE_DIO_WRITE);
		}
	}
	fuse_invalidate_attr(inode);
	if (res > 0)
		fuse_write_update_size(inode, iocb->ki_pos);
	inode_unlock(inode);

	return res;
}