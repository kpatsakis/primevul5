static int decode_attr_time(struct xdr_stream *xdr, struct timespec64 *time)
{
	__be32 *p;

	p = xdr_inline_decode(xdr, nfstime4_maxsz << 2);
	if (unlikely(!p))
		return -EIO;
	xdr_decode_nfstime4(p, time);
	return 0;
}