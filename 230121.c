decode_bitmap4(struct xdr_stream *xdr, uint32_t *bitmap, size_t sz)
{
	ssize_t ret;

	ret = xdr_stream_decode_uint32_array(xdr, bitmap, sz);
	if (likely(ret >= 0))
		return ret;
	if (ret != -EMSGSIZE)
		return -EIO;
	return sz;
}