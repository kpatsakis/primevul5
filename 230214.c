static bool __decode_op_hdr(struct xdr_stream *xdr, enum nfs_opnum4 expected,
		int *nfs_retval)
{
	__be32 *p;
	uint32_t opnum;
	int32_t nfserr;

	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		goto out_overflow;
	opnum = be32_to_cpup(p++);
	if (unlikely(opnum != expected))
		goto out_bad_operation;
	if (unlikely(*p != cpu_to_be32(NFS_OK)))
		goto out_status;
	*nfs_retval = 0;
	return true;
out_status:
	nfserr = be32_to_cpup(p);
	trace_nfs4_xdr_status(xdr, opnum, nfserr);
	*nfs_retval = nfs4_stat_to_errno(nfserr);
	return true;
out_bad_operation:
	dprintk("nfs: Server returned operation"
		" %d but we issued a request for %d\n",
			opnum, expected);
	*nfs_retval = -EREMOTEIO;
	return false;
out_overflow:
	*nfs_retval = -EIO;
	return false;
}