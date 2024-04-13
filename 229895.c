static ssize_t xdr_encode_bitmap4(struct xdr_stream *xdr,
		const __u32 *bitmap, size_t len)
{
	ssize_t ret;

	/* Trim empty words */
	while (len > 0 && bitmap[len-1] == 0)
		len--;
	ret = xdr_stream_encode_uint32_array(xdr, bitmap, len);
	if (WARN_ON_ONCE(ret < 0))
		return ret;
	return len;
}