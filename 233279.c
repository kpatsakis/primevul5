static void __write_bytes_to_xdr_buf(const struct xdr_buf *subbuf,
				     void *obj, unsigned int len)
{
	unsigned int this_len;

	this_len = min_t(unsigned int, len, subbuf->head[0].iov_len);
	memcpy(subbuf->head[0].iov_base, obj, this_len);
	len -= this_len;
	obj += this_len;
	this_len = min_t(unsigned int, len, subbuf->page_len);
	_copy_to_pages(subbuf->pages, subbuf->page_base, obj, this_len);
	len -= this_len;
	obj += this_len;
	this_len = min_t(unsigned int, len, subbuf->tail[0].iov_len);
	memcpy(subbuf->tail[0].iov_base, obj, this_len);
}