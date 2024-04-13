static int decode_attr_bitmap(struct xdr_stream *xdr, uint32_t *bitmap)
{
	ssize_t ret;
	ret = decode_bitmap4(xdr, bitmap, 3);
	return ret < 0 ? ret : 0;
}