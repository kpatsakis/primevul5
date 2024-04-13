int xdr_encode_word(const struct xdr_buf *buf, unsigned int base, u32 obj)
{
	__be32	raw = cpu_to_be32(obj);

	return write_bytes_to_xdr_buf(buf, base, &raw, sizeof(obj));
}