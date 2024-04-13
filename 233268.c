int xdr_process_buf(const struct xdr_buf *buf, unsigned int offset,
		    unsigned int len,
		    int (*actor)(struct scatterlist *, void *), void *data)
{
	int i, ret = 0;
	unsigned int page_len, thislen, page_offset;
	struct scatterlist      sg[1];

	sg_init_table(sg, 1);

	if (offset >= buf->head[0].iov_len) {
		offset -= buf->head[0].iov_len;
	} else {
		thislen = buf->head[0].iov_len - offset;
		if (thislen > len)
			thislen = len;
		sg_set_buf(sg, buf->head[0].iov_base + offset, thislen);
		ret = actor(sg, data);
		if (ret)
			goto out;
		offset = 0;
		len -= thislen;
	}
	if (len == 0)
		goto out;

	if (offset >= buf->page_len) {
		offset -= buf->page_len;
	} else {
		page_len = buf->page_len - offset;
		if (page_len > len)
			page_len = len;
		len -= page_len;
		page_offset = (offset + buf->page_base) & (PAGE_SIZE - 1);
		i = (offset + buf->page_base) >> PAGE_SHIFT;
		thislen = PAGE_SIZE - page_offset;
		do {
			if (thislen > page_len)
				thislen = page_len;
			sg_set_page(sg, buf->pages[i], thislen, page_offset);
			ret = actor(sg, data);
			if (ret)
				goto out;
			page_len -= thislen;
			i++;
			page_offset = 0;
			thislen = PAGE_SIZE;
		} while (page_len != 0);
		offset = 0;
	}
	if (len == 0)
		goto out;
	if (offset < buf->tail[0].iov_len) {
		thislen = buf->tail[0].iov_len - offset;
		if (thislen > len)
			thislen = len;
		sg_set_buf(sg, buf->tail[0].iov_base + offset, thislen);
		ret = actor(sg, data);
		len -= thislen;
	}
	if (len != 0)
		ret = -EINVAL;
out:
	return ret;
}