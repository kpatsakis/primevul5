static int fuse_verify_ioctl_iov(struct fuse_conn *fc, struct iovec *iov,
				 size_t count)
{
	size_t n;
	u32 max = fc->max_pages << PAGE_SHIFT;

	for (n = 0; n < count; n++, iov++) {
		if (iov->iov_len > (size_t) max)
			return -ENOMEM;
		max -= iov->iov_len;
	}
	return 0;
}