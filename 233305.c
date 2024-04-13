static void __read_bytes_from_xdr_buf(const struct xdr_buf *subbuf,
				      void *obj, unsigned int len)
{
	unsigned int this_len;

	this_len = min_t(unsigned int, len, subbuf->head[0].iov_len);
	memcpy(obj, subbuf->head[0].iov_base, this_len);
	len -= this_len;
	obj += this_len;
	this_len = min_t(unsigned int, len, subbuf->page_len);
	_copy_from_pages(obj, subbuf->pages, subbuf->page_base, this_len);
	len -= this_len;
	obj += this_len;
	this_len = min_t(unsigned int, len, subbuf->tail[0].iov_len);
	memcpy(obj, subbuf->tail[0].iov_base, this_len);
}