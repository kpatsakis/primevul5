static void nfs4_bitmap_copy_adjust_setattr(__u32 *dst,
		const __u32 *src, struct inode *inode)
{
	nfs4_bitmap_copy_adjust(dst, src, inode);
}