static int decode_attr_aclsupport(struct xdr_stream *xdr, uint32_t *bitmap, uint32_t *res)
{
	__be32 *p;

	*res = 0;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_ACLSUPPORT - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_ACLSUPPORT)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			return -EIO;
		*res = be32_to_cpup(p);
		bitmap[0] &= ~FATTR4_WORD0_ACLSUPPORT;
	}
	dprintk("%s: ACLs supported=%u\n", __func__, (unsigned int)*res);
	return 0;
}