static int decode_setclientid(struct xdr_stream *xdr, struct nfs4_setclientid_res *res)
{
	__be32 *p;
	uint32_t opnum;
	int32_t nfserr;

	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		return -EIO;
	opnum = be32_to_cpup(p++);
	if (opnum != OP_SETCLIENTID) {
		dprintk("nfs: decode_setclientid: Server returned operation"
			" %d\n", opnum);
		return -EIO;
	}
	nfserr = be32_to_cpup(p);
	if (nfserr == NFS_OK) {
		p = xdr_inline_decode(xdr, 8 + NFS4_VERIFIER_SIZE);
		if (unlikely(!p))
			return -EIO;
		p = xdr_decode_hyper(p, &res->clientid);
		memcpy(res->confirm.data, p, NFS4_VERIFIER_SIZE);
	} else if (nfserr == NFSERR_CLID_INUSE) {
		uint32_t len;

		/* skip netid string */
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			return -EIO;
		len = be32_to_cpup(p);
		p = xdr_inline_decode(xdr, len);
		if (unlikely(!p))
			return -EIO;

		/* skip uaddr string */
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			return -EIO;
		len = be32_to_cpup(p);
		p = xdr_inline_decode(xdr, len);
		if (unlikely(!p))
			return -EIO;
		return -NFSERR_CLID_INUSE;
	} else
		return nfs4_stat_to_errno(nfserr);

	return 0;
}