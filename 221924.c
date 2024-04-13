static ssize_t __fuse_direct_read(struct fuse_io_priv *io,
				  struct iov_iter *iter,
				  loff_t *ppos)
{
	ssize_t res;
	struct inode *inode = file_inode(io->iocb->ki_filp);

	res = fuse_direct_io(io, iter, ppos, 0);

	fuse_invalidate_atime(inode);

	return res;
}