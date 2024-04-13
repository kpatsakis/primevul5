static int decode_attr_xattrsupport(struct xdr_stream *xdr, uint32_t *bitmap,
				    uint32_t *res)
{
	__be32 *p;

	*res = 0;
	if (unlikely(bitmap[2] & (FATTR4_WORD2_XATTR_SUPPORT - 1U)))
		return -EIO;
	if (likely(bitmap[2] & FATTR4_WORD2_XATTR_SUPPORT)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			return -EIO;
		*res = be32_to_cpup(p);
		bitmap[2] &= ~FATTR4_WORD2_XATTR_SUPPORT;
	}
	dprintk("%s: XATTR support=%s\n", __func__,
		*res == 0 ? "false" : "true");
	return 0;
}