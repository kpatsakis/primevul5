static int decode_pnfs_layout_types(struct xdr_stream *xdr,
				    struct nfs_fsinfo *fsinfo)
{
	__be32 *p;
	uint32_t i;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	fsinfo->nlayouttypes = be32_to_cpup(p);

	/* pNFS is not supported by the underlying file system */
	if (fsinfo->nlayouttypes == 0)
		return 0;

	/* Decode and set first layout type, move xdr->p past unused types */
	p = xdr_inline_decode(xdr, fsinfo->nlayouttypes * 4);
	if (unlikely(!p))
		return -EIO;

	/* If we get too many, then just cap it at the max */
	if (fsinfo->nlayouttypes > NFS_MAX_LAYOUT_TYPES) {
		printk(KERN_INFO "NFS: %s: Warning: Too many (%u) pNFS layout types\n",
			__func__, fsinfo->nlayouttypes);
		fsinfo->nlayouttypes = NFS_MAX_LAYOUT_TYPES;
	}

	for(i = 0; i < fsinfo->nlayouttypes; ++i)
		fsinfo->layouttype[i] = be32_to_cpup(p++);
	return 0;
}