static int decode_open(struct xdr_stream *xdr, struct nfs_openres *res)
{
	__be32 *p;
	uint32_t savewords, bmlen, i;
	int status;

	if (!__decode_op_hdr(xdr, OP_OPEN, &status))
		return status;
	nfs_increment_open_seqid(status, res->seqid);
	if (status)
		return status;
	status = decode_open_stateid(xdr, &res->stateid);
	if (unlikely(status))
		return status;

	decode_change_info(xdr, &res->cinfo);

	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		return -EIO;
	res->rflags = be32_to_cpup(p++);
	bmlen = be32_to_cpup(p);
	if (bmlen > 10)
		goto xdr_error;

	p = xdr_inline_decode(xdr, bmlen << 2);
	if (unlikely(!p))
		return -EIO;
	savewords = min_t(uint32_t, bmlen, NFS4_BITMAP_SIZE);
	for (i = 0; i < savewords; ++i)
		res->attrset[i] = be32_to_cpup(p++);
	for (; i < NFS4_BITMAP_SIZE; i++)
		res->attrset[i] = 0;

	return decode_delegation(xdr, res);
xdr_error:
	dprintk("%s: Bitmap too large! Length = %u\n", __func__, bmlen);
	return -EIO;
}