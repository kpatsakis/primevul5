static void tree_insert(struct rb_root *root, struct fuse_writepage_args *wpa)
{
	WARN_ON(fuse_insert_writeback(root, wpa));
}