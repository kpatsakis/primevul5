static void encode_fsinfo(struct xdr_stream *xdr, const u32* bitmask, struct compound_hdr *hdr)
{
	encode_getattr(xdr, nfs4_fsinfo_bitmap, bitmask,
			ARRAY_SIZE(nfs4_fsinfo_bitmap), hdr);
}