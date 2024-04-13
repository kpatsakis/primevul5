static int decode_no_delegation(struct xdr_stream *xdr, struct nfs_openres *res)
{
	__be32 *p;
	uint32_t why_no_delegation;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	why_no_delegation = be32_to_cpup(p);
	switch (why_no_delegation) {
		case WND4_CONTENTION:
		case WND4_RESOURCE:
			xdr_inline_decode(xdr, 4);
			/* Ignore for now */
	}
	return 0;
}