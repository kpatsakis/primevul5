bool nfs4_write_need_cache_consistency_data(struct nfs_pgio_header *hdr)
{
	/* Don't request attributes for pNFS or O_DIRECT writes */
	if (hdr->ds_clp != NULL || hdr->dreq != NULL)
		return false;
	/* Otherwise, request attributes if and only if we don't hold
	 * a delegation
	 */
	return nfs4_have_delegation(hdr->inode, FMODE_READ) == 0;
}