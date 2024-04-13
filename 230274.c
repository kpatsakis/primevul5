static int decode_free_stateid(struct xdr_stream *xdr,
			       struct nfs41_free_stateid_res *res)
{
	res->status = decode_op_hdr(xdr, OP_FREE_STATEID);
	return res->status;
}