static inline int register_fuseblk(void)
{
	return register_filesystem(&fuseblk_fs_type);
}