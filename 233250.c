static void xdr_buf_iov_zero(const struct kvec *iov, unsigned int base,
			     unsigned int len)
{
	if (base >= iov->iov_len)
		return;
	if (len > iov->iov_len - base)
		len = iov->iov_len - base;
	memset(iov->iov_base + base, 0, len);
}