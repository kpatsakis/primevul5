static void nfs4_bitmap_copy_adjust(__u32 *dst, const __u32 *src,
		struct inode *inode)
{
	unsigned long cache_validity;

	memcpy(dst, src, NFS4_BITMASK_SZ*sizeof(*dst));
	if (!inode || !nfs4_have_delegation(inode, FMODE_READ))
		return;

	cache_validity = READ_ONCE(NFS_I(inode)->cache_validity);
	if (!(cache_validity & NFS_INO_REVAL_FORCED))
		cache_validity &= ~(NFS_INO_INVALID_CHANGE
				| NFS_INO_INVALID_SIZE);

	if (!(cache_validity & NFS_INO_INVALID_SIZE))
		dst[0] &= ~FATTR4_WORD0_SIZE;

	if (!(cache_validity & NFS_INO_INVALID_CHANGE))
		dst[0] &= ~FATTR4_WORD0_CHANGE;
}