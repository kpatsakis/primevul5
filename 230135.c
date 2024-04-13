static int decode_test_stateid(struct xdr_stream *xdr,
			       struct nfs41_test_stateid_res *res)
{
	__be32 *p;
	int status;
	int num_res;

	status = decode_op_hdr(xdr, OP_TEST_STATEID);
	if (status)
		return status;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	num_res = be32_to_cpup(p++);
	if (num_res != 1)
		return -EIO;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	res->status = be32_to_cpup(p++);

	return status;
}