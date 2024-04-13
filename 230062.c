nfs4_dec_nlink_locked(struct inode *inode)
{
	NFS_I(inode)->cache_validity |= NFS_INO_INVALID_OTHER;
	drop_nlink(inode);
}