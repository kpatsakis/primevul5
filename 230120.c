nfs4_update_changeattr(struct inode *dir, struct nfs4_change_info *cinfo,
		unsigned long timestamp, unsigned long cache_validity)
{
	spin_lock(&dir->i_lock);
	nfs4_update_changeattr_locked(dir, cinfo, timestamp, cache_validity);
	spin_unlock(&dir->i_lock);
}