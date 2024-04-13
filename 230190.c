static int decode_attr_supported(struct xdr_stream *xdr, uint32_t *bitmap, uint32_t *bitmask)
{
	if (likely(bitmap[0] & FATTR4_WORD0_SUPPORTED_ATTRS)) {
		int ret;
		ret = decode_attr_bitmap(xdr, bitmask);
		if (unlikely(ret < 0))
			return ret;
		bitmap[0] &= ~FATTR4_WORD0_SUPPORTED_ATTRS;
	} else
		bitmask[0] = bitmask[1] = bitmask[2] = 0;
	dprintk("%s: bitmask=%08x:%08x:%08x\n", __func__,
		bitmask[0], bitmask[1], bitmask[2]);
	return 0;
}