static int nfs4_check_cl_exchange_flags(u32 flags)
{
	if (flags & ~EXCHGID4_FLAG_MASK_R)
		goto out_inval;
	if ((flags & EXCHGID4_FLAG_USE_PNFS_MDS) &&
	    (flags & EXCHGID4_FLAG_USE_NON_PNFS))
		goto out_inval;
	if (!(flags & (EXCHGID4_FLAG_MASK_PNFS)))
		goto out_inval;
	return NFS_OK;
out_inval:
	return -NFS4ERR_INVAL;
}