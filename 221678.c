static inline void unregister_fuseblk(void)
{
	unregister_filesystem(&fuseblk_fs_type);
}