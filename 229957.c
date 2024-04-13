static inline int nfs4_lock_type(struct file_lock *fl, int block)
{
	if (fl->fl_type == F_RDLCK)
		return block ? NFS4_READW_LT : NFS4_READ_LT;
	return block ? NFS4_WRITEW_LT : NFS4_WRITE_LT;
}