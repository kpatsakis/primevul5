static struct vfsmount *fuse_dentry_automount(struct path *path)
{
	struct fs_context *fsc;
	struct fuse_mount *parent_fm = get_fuse_mount_super(path->mnt->mnt_sb);
	struct fuse_conn *fc = parent_fm->fc;
	struct fuse_mount *fm;
	struct vfsmount *mnt;
	struct fuse_inode *mp_fi = get_fuse_inode(d_inode(path->dentry));
	struct super_block *sb;
	int err;

	fsc = fs_context_for_submount(path->mnt->mnt_sb->s_type, path->dentry);
	if (IS_ERR(fsc)) {
		err = PTR_ERR(fsc);
		goto out;
	}

	err = -ENOMEM;
	fm = kzalloc(sizeof(struct fuse_mount), GFP_KERNEL);
	if (!fm)
		goto out_put_fsc;

	fsc->s_fs_info = fm;
	sb = sget_fc(fsc, NULL, set_anon_super_fc);
	if (IS_ERR(sb)) {
		err = PTR_ERR(sb);
		kfree(fm);
		goto out_put_fsc;
	}
	fm->fc = fuse_conn_get(fc);

	/* Initialize superblock, making @mp_fi its root */
	err = fuse_fill_super_submount(sb, mp_fi);
	if (err)
		goto out_put_sb;

	sb->s_flags |= SB_ACTIVE;
	fsc->root = dget(sb->s_root);
	/* We are done configuring the superblock, so unlock it */
	up_write(&sb->s_umount);

	down_write(&fc->killsb);
	list_add_tail(&fm->fc_entry, &fc->mounts);
	up_write(&fc->killsb);

	/* Create the submount */
	mnt = vfs_create_mount(fsc);
	if (IS_ERR(mnt)) {
		err = PTR_ERR(mnt);
		goto out_put_fsc;
	}
	mntget(mnt);
	put_fs_context(fsc);
	return mnt;

out_put_sb:
	/*
	 * Only jump here when fsc->root is NULL and sb is still locked
	 * (otherwise put_fs_context() will put the superblock)
	 */
	deactivate_locked_super(sb);
out_put_fsc:
	put_fs_context(fsc);
out:
	return ERR_PTR(err);
}