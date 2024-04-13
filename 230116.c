static void encode_fs_locations(struct xdr_stream *xdr, const u32* bitmask, struct compound_hdr *hdr)
{
	encode_getattr(xdr, nfs4_fs_locations_bitmap, bitmask,
			ARRAY_SIZE(nfs4_fs_locations_bitmap), hdr);
}