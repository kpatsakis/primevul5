xdr_free_bvec(struct xdr_buf *buf)
{
	kfree(buf->bvec);
	buf->bvec = NULL;
}