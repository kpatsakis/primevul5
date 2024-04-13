xdr_alloc_bvec(struct xdr_buf *buf, gfp_t gfp)
{
	size_t i, n = xdr_buf_pagecount(buf);

	if (n != 0 && buf->bvec == NULL) {
		buf->bvec = kmalloc_array(n, sizeof(buf->bvec[0]), gfp);
		if (!buf->bvec)
			return -ENOMEM;
		for (i = 0; i < n; i++) {
			buf->bvec[i].bv_page = buf->pages[i];
			buf->bvec[i].bv_len = PAGE_SIZE;
			buf->bvec[i].bv_offset = 0;
		}
	}
	return 0;
}