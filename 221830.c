static void fuse_invalidate_attr_mask(struct inode *inode, u32 mask)
{
	set_mask_bits(&get_fuse_inode(inode)->inval_mask, 0, mask);
}