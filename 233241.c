int xdr_decode_array2(const struct xdr_buf *buf, unsigned int base,
		      struct xdr_array2_desc *desc)
{
	if (base >= buf->len)
		return -EINVAL;

	return xdr_xcode_array2(buf, base, desc, 0);
}