nfs4_update_changeattr_locked(struct inode *inode,
		struct nfs4_change_info *cinfo,
		unsigned long timestamp, unsigned long cache_validity)
{
	struct nfs_inode *nfsi = NFS_I(inode);

	nfsi->cache_validity |= NFS_INO_INVALID_CTIME
		| NFS_INO_INVALID_MTIME
		| cache_validity;

	if (cinfo->atomic && cinfo->before == inode_peek_iversion_raw(inode)) {
		nfsi->cache_validity &= ~NFS_INO_REVAL_PAGECACHE;
		nfsi->attrtimeo_timestamp = jiffies;
	} else {
		if (S_ISDIR(inode->i_mode)) {
			nfsi->cache_validity |= NFS_INO_INVALID_DATA;
			nfs_force_lookup_revalidate(inode);
		} else {
			if (!NFS_PROTO(inode)->have_delegation(inode,
							       FMODE_READ))
				nfsi->cache_validity |= NFS_INO_REVAL_PAGECACHE;
		}

		if (cinfo->before != inode_peek_iversion_raw(inode))
			nfsi->cache_validity |= NFS_INO_INVALID_ACCESS |
						NFS_INO_INVALID_ACL |
						NFS_INO_INVALID_XATTR;
	}
	inode_set_iversion_raw(inode, cinfo->after);
	nfsi->read_cache_jiffies = timestamp;
	nfsi->attr_gencount = nfs_inc_attr_generation_counter();
	nfsi->cache_validity &= ~NFS_INO_INVALID_CHANGE;

	if (nfsi->cache_validity & NFS_INO_INVALID_DATA)
		nfs_fscache_invalidate(inode);
}