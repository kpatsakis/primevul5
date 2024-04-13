static void encode_compound_hdr(struct xdr_stream *xdr,
				struct rpc_rqst *req,
				struct compound_hdr *hdr)
{
	__be32 *p;

	/* initialize running count of expected bytes in reply.
	 * NOTE: the replied tag SHOULD be the same is the one sent,
	 * but this is not required as a MUST for the server to do so. */
	hdr->replen = 3 + hdr->taglen;

	WARN_ON_ONCE(hdr->taglen > NFS4_MAXTAGLEN);
	encode_string(xdr, hdr->taglen, hdr->tag);
	p = reserve_space(xdr, 8);
	*p++ = cpu_to_be32(hdr->minorversion);
	hdr->nops_p = p;
	*p = cpu_to_be32(hdr->nops);
}