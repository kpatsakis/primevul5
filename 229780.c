static int decode_rw_delegation(struct xdr_stream *xdr,
		uint32_t delegation_type,
		struct nfs_openres *res)
{
	__be32 *p;
	int status;

	status = decode_delegation_stateid(xdr, &res->delegation);
	if (unlikely(status))
		return status;
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	res->do_recall = be32_to_cpup(p);

	switch (delegation_type) {
	case NFS4_OPEN_DELEGATE_READ:
		res->delegation_type = FMODE_READ;
		break;
	case NFS4_OPEN_DELEGATE_WRITE:
		res->delegation_type = FMODE_WRITE|FMODE_READ;
		if (decode_space_limit(xdr, &res->pagemod_limit) < 0)
				return -EIO;
	}
	return decode_ace(xdr, NULL);
}