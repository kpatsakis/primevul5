static int decode_create_session(struct xdr_stream *xdr,
				 struct nfs41_create_session_res *res)
{
	__be32 *p;
	int status;

	status = decode_op_hdr(xdr, OP_CREATE_SESSION);
	if (!status)
		status = decode_sessionid(xdr, &res->sessionid);
	if (unlikely(status))
		return status;

	/* seqid, flags */
	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		return -EIO;
	res->seqid = be32_to_cpup(p++);
	res->flags = be32_to_cpup(p);

	/* Channel attributes */
	status = decode_chan_attrs(xdr, &res->fc_attrs);
	if (!status)
		status = decode_chan_attrs(xdr, &res->bc_attrs);
	return status;
}