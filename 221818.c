static void fuse_free_inode(struct inode *inode)
{
	struct fuse_inode *fi = get_fuse_inode(inode);

	mutex_destroy(&fi->mutex);
	kfree(fi->forget);
#ifdef CONFIG_FUSE_DAX
	kfree(fi->dax);
#endif
	kmem_cache_free(fuse_inode_cachep, fi);
}