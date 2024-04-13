static int ovl_fill_super(struct super_block *sb, void *data, int silent)
{
	struct path lowerpath;
	struct path upperpath;
	struct path workpath;
	struct inode *root_inode;
	struct dentry *root_dentry;
	struct ovl_entry *oe;
	struct ovl_fs *ufs;
	struct kstatfs statfs;
	int err;

	err = -ENOMEM;
	ufs = kzalloc(sizeof(struct ovl_fs), GFP_KERNEL);
	if (!ufs)
		goto out;

	err = ovl_parse_opt((char *) data, &ufs->config);
	if (err)
		goto out_free_config;

	/* FIXME: workdir is not needed for a R/O mount */
	err = -EINVAL;
	if (!ufs->config.upperdir || !ufs->config.lowerdir ||
	    !ufs->config.workdir) {
		pr_err("overlayfs: missing upperdir or lowerdir or workdir\n");
		goto out_free_config;
	}

	err = -ENOMEM;
	oe = ovl_alloc_entry();
	if (oe == NULL)
		goto out_free_config;

	err = ovl_mount_dir(ufs->config.upperdir, &upperpath);
	if (err)
		goto out_free_oe;

	err = ovl_mount_dir(ufs->config.lowerdir, &lowerpath);
	if (err)
		goto out_put_upperpath;

	err = ovl_mount_dir(ufs->config.workdir, &workpath);
	if (err)
		goto out_put_lowerpath;

	err = -EINVAL;
	if (!S_ISDIR(upperpath.dentry->d_inode->i_mode) ||
	    !S_ISDIR(lowerpath.dentry->d_inode->i_mode) ||
	    !S_ISDIR(workpath.dentry->d_inode->i_mode)) {
		pr_err("overlayfs: upperdir or lowerdir or workdir not a directory\n");
		goto out_put_workpath;
	}

	if (upperpath.mnt != workpath.mnt) {
		pr_err("overlayfs: workdir and upperdir must reside under the same mount\n");
		goto out_put_workpath;
	}
	if (!ovl_workdir_ok(workpath.dentry, upperpath.dentry)) {
		pr_err("overlayfs: workdir and upperdir must be separate subtrees\n");
		goto out_put_workpath;
	}

	if (!ovl_is_allowed_fs_type(upperpath.dentry)) {
		pr_err("overlayfs: filesystem of upperdir is not supported\n");
		goto out_put_workpath;
	}

	if (!ovl_is_allowed_fs_type(lowerpath.dentry)) {
		pr_err("overlayfs: filesystem of lowerdir is not supported\n");
		goto out_put_workpath;
	}

	err = vfs_statfs(&lowerpath, &statfs);
	if (err) {
		pr_err("overlayfs: statfs failed on lowerpath\n");
		goto out_put_workpath;
 	}
 	ufs->lower_namelen = statfs.f_namelen;
 
 	ufs->upper_mnt = clone_private_mount(&upperpath);
 	err = PTR_ERR(ufs->upper_mnt);
 	if (IS_ERR(ufs->upper_mnt)) {
		pr_err("overlayfs: failed to clone upperpath\n");
		goto out_put_workpath;
	}

	ufs->lower_mnt = clone_private_mount(&lowerpath);
	err = PTR_ERR(ufs->lower_mnt);
	if (IS_ERR(ufs->lower_mnt)) {
		pr_err("overlayfs: failed to clone lowerpath\n");
		goto out_put_upper_mnt;
	}

	ufs->workdir = ovl_workdir_create(ufs->upper_mnt, workpath.dentry);
	err = PTR_ERR(ufs->workdir);
	if (IS_ERR(ufs->workdir)) {
		pr_err("overlayfs: failed to create directory %s/%s\n",
		       ufs->config.workdir, OVL_WORKDIR_NAME);
		goto out_put_lower_mnt;
	}

	/*
	 * Make lower_mnt R/O.  That way fchmod/fchown on lower file
	 * will fail instead of modifying lower fs.
	 */
	ufs->lower_mnt->mnt_flags |= MNT_READONLY;

	/* If the upper fs is r/o, we mark overlayfs r/o too */
	if (ufs->upper_mnt->mnt_sb->s_flags & MS_RDONLY)
		sb->s_flags |= MS_RDONLY;

	sb->s_d_op = &ovl_dentry_operations;

	err = -ENOMEM;
	root_inode = ovl_new_inode(sb, S_IFDIR, oe);
	if (!root_inode)
		goto out_put_workdir;

	root_dentry = d_make_root(root_inode);
	if (!root_dentry)
		goto out_put_workdir;

	mntput(upperpath.mnt);
	mntput(lowerpath.mnt);
	path_put(&workpath);

	oe->__upperdentry = upperpath.dentry;
	oe->lowerdentry = lowerpath.dentry;

	root_dentry->d_fsdata = oe;

	sb->s_magic = OVERLAYFS_SUPER_MAGIC;
	sb->s_op = &ovl_super_operations;
	sb->s_root = root_dentry;
	sb->s_fs_info = ufs;

	return 0;

out_put_workdir:
	dput(ufs->workdir);
out_put_lower_mnt:
	mntput(ufs->lower_mnt);
out_put_upper_mnt:
	mntput(ufs->upper_mnt);
out_put_workpath:
	path_put(&workpath);
out_put_lowerpath:
	path_put(&lowerpath);
out_put_upperpath:
	path_put(&upperpath);
out_free_oe:
	kfree(oe);
out_free_config:
	kfree(ufs->config.lowerdir);
	kfree(ufs->config.upperdir);
	kfree(ufs->config.workdir);
	kfree(ufs);
out:
	return err;
}
