static void encode_getfattr_open(struct xdr_stream *xdr, const u32 *bitmask,
				 const u32 *open_bitmap,
				 struct compound_hdr *hdr)
{
	encode_getattr(xdr, open_bitmap, bitmask, 3, hdr);
}