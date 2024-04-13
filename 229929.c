static ssize_t decode_nfs4_string(struct xdr_stream *xdr,
		struct nfs4_string *name, gfp_t gfp_flags)
{
	ssize_t ret;

	ret = xdr_stream_decode_string_dup(xdr, &name->data,
			XDR_MAX_NETOBJ, gfp_flags);
	name->len = 0;
	if (ret > 0)
		name->len = ret;
	return ret;
}