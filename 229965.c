static int decode_write(struct xdr_stream *xdr, struct nfs_pgio_res *res)
{
	__be32 *p;
	int status;

	status = decode_op_hdr(xdr, OP_WRITE);
	if (status)
		return status;

	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		return -EIO;
	res->count = be32_to_cpup(p++);
	res->verf->committed = be32_to_cpup(p++);
	return decode_write_verifier(xdr, &res->verf->verifier);
}