static int decode_attr_pnfstype(struct xdr_stream *xdr, uint32_t *bitmap,
				struct nfs_fsinfo *fsinfo)
{
	int status = 0;

	dprintk("%s: bitmap is %x\n", __func__, bitmap[1]);
	if (unlikely(bitmap[1] & (FATTR4_WORD1_FS_LAYOUT_TYPES - 1U)))
		return -EIO;
	if (bitmap[1] & FATTR4_WORD1_FS_LAYOUT_TYPES) {
		status = decode_pnfs_layout_types(xdr, fsinfo);
		bitmap[1] &= ~FATTR4_WORD1_FS_LAYOUT_TYPES;
	}
	return status;
}