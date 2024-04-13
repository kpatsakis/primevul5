static void xdr_buf_try_expand(struct xdr_buf *buf, unsigned int len)
{
	struct kvec *head = buf->head;
	struct kvec *tail = buf->tail;
	unsigned int sum = head->iov_len + buf->page_len + tail->iov_len;
	unsigned int free_space, newlen;

	if (sum > buf->len) {
		free_space = min_t(unsigned int, sum - buf->len, len);
		newlen = xdr_buf_pages_fill_sparse(buf, buf->len + free_space,
						   GFP_KERNEL);
		free_space = newlen - buf->len;
		buf->len = newlen;
		len -= free_space;
		if (!len)
			return;
	}

	if (buf->buflen > sum) {
		/* Expand the tail buffer */
		free_space = min_t(unsigned int, buf->buflen - sum, len);
		tail->iov_len += free_space;
		buf->len += free_space;
	}
}