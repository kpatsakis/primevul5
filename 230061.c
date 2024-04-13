static int decode_delegation(struct xdr_stream *xdr, struct nfs_openres *res)
{
	__be32 *p;
	uint32_t delegation_type;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	delegation_type = be32_to_cpup(p);
	res->delegation_type = 0;
	switch (delegation_type) {
	case NFS4_OPEN_DELEGATE_NONE:
		return 0;
	case NFS4_OPEN_DELEGATE_READ:
	case NFS4_OPEN_DELEGATE_WRITE:
		return decode_rw_delegation(xdr, delegation_type, res);
	case NFS4_OPEN_DELEGATE_NONE_EXT:
		return decode_no_delegation(xdr, res);
	}
	return -EIO;
}