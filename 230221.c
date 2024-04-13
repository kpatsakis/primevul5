static int decode_attr_maxlink(struct xdr_stream *xdr, uint32_t *bitmap, uint32_t *maxlink)
{
	__be32 *p;
	int status = 0;

	*maxlink = 1;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_MAXLINK - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_MAXLINK)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			return -EIO;
		*maxlink = be32_to_cpup(p);
		bitmap[0] &= ~FATTR4_WORD0_MAXLINK;
	}
	dprintk("%s: maxlink=%u\n", __func__, *maxlink);
	return status;
}