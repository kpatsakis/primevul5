static int decode_access(struct xdr_stream *xdr, u32 *supported, u32 *access)
{
	__be32 *p;
	uint32_t supp, acc;
	int status;

	status = decode_op_hdr(xdr, OP_ACCESS);
	if (status)
		return status;
	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		return -EIO;
	supp = be32_to_cpup(p++);
	acc = be32_to_cpup(p);
	*supported = supp;
	*access = acc;
	return 0;
}