static int decode_attr_exclcreat_supported(struct xdr_stream *xdr,
				 uint32_t *bitmap, uint32_t *bitmask)
{
	if (likely(bitmap[2] & FATTR4_WORD2_SUPPATTR_EXCLCREAT)) {
		int ret;
		ret = decode_attr_bitmap(xdr, bitmask);
		if (unlikely(ret < 0))
			return ret;
		bitmap[2] &= ~FATTR4_WORD2_SUPPATTR_EXCLCREAT;
	} else
		bitmask[0] = bitmask[1] = bitmask[2] = 0;
	dprintk("%s: bitmask=%08x:%08x:%08x\n", __func__,
		bitmask[0], bitmask[1], bitmask[2]);
	return 0;
}