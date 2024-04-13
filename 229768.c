static void encode_getfattr(struct xdr_stream *xdr, const u32* bitmask, struct compound_hdr *hdr)
{
	encode_getattr(xdr, nfs4_fattr_bitmap, bitmask,
			ARRAY_SIZE(nfs4_fattr_bitmap), hdr);
}