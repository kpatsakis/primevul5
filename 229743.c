static int decode_ace(struct xdr_stream *xdr, void *ace)
{
	__be32 *p;
	unsigned int strlen;
	char *str;

	p = xdr_inline_decode(xdr, 12);
	if (unlikely(!p))
		return -EIO;
	return decode_opaque_inline(xdr, &strlen, &str);
}