static struct inode *fuse_alloc_inode(struct super_block *sb)
{
	struct fuse_inode *fi;

	fi = kmem_cache_alloc(fuse_inode_cachep, GFP_KERNEL);
	if (!fi)
		return NULL;

	fi->i_time = 0;
	fi->inval_mask = 0;
	fi->nodeid = 0;
	fi->nlookup = 0;
	fi->attr_version = 0;
	fi->orig_ino = 0;
	fi->state = 0;
	mutex_init(&fi->mutex);
	init_rwsem(&fi->i_mmap_sem);
	spin_lock_init(&fi->lock);
	fi->forget = fuse_alloc_forget();
	if (!fi->forget)
		goto out_free;

	if (IS_ENABLED(CONFIG_FUSE_DAX) && !fuse_dax_inode_alloc(sb, fi))
		goto out_free_forget;

	return &fi->inode;

out_free_forget:
	kfree(fi->forget);
out_free:
	kmem_cache_free(fuse_inode_cachep, fi);
	return NULL;
}