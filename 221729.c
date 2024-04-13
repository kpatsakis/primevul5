static int fuse_create_open(struct inode *dir, struct dentry *entry,
			    struct file *file, unsigned flags,
			    umode_t mode)
{
	int err;
	struct inode *inode;
	struct fuse_mount *fm = get_fuse_mount(dir);
	FUSE_ARGS(args);
	struct fuse_forget_link *forget;
	struct fuse_create_in inarg;
	struct fuse_open_out outopen;
	struct fuse_entry_out outentry;
	struct fuse_inode *fi;
	struct fuse_file *ff;

	/* Userspace expects S_IFREG in create mode */
	BUG_ON((mode & S_IFMT) != S_IFREG);

	forget = fuse_alloc_forget();
	err = -ENOMEM;
	if (!forget)
		goto out_err;

	err = -ENOMEM;
	ff = fuse_file_alloc(fm);
	if (!ff)
		goto out_put_forget_req;

	if (!fm->fc->dont_mask)
		mode &= ~current_umask();

	flags &= ~O_NOCTTY;
	memset(&inarg, 0, sizeof(inarg));
	memset(&outentry, 0, sizeof(outentry));
	inarg.flags = flags;
	inarg.mode = mode;
	inarg.umask = current_umask();

	if (fm->fc->handle_killpriv_v2 && (flags & O_TRUNC) &&
	    !(flags & O_EXCL) && !capable(CAP_FSETID)) {
		inarg.open_flags |= FUSE_OPEN_KILL_SUIDGID;
	}

	args.opcode = FUSE_CREATE;
	args.nodeid = get_node_id(dir);
	args.in_numargs = 2;
	args.in_args[0].size = sizeof(inarg);
	args.in_args[0].value = &inarg;
	args.in_args[1].size = entry->d_name.len + 1;
	args.in_args[1].value = entry->d_name.name;
	args.out_numargs = 2;
	args.out_args[0].size = sizeof(outentry);
	args.out_args[0].value = &outentry;
	args.out_args[1].size = sizeof(outopen);
	args.out_args[1].value = &outopen;
	err = fuse_simple_request(fm, &args);
	if (err)
		goto out_free_ff;

	err = -EIO;
	if (!S_ISREG(outentry.attr.mode) || invalid_nodeid(outentry.nodeid) ||
	    fuse_invalid_attr(&outentry.attr))
		goto out_free_ff;

	ff->fh = outopen.fh;
	ff->nodeid = outentry.nodeid;
	ff->open_flags = outopen.open_flags;
	inode = fuse_iget(dir->i_sb, outentry.nodeid, outentry.generation,
			  &outentry.attr, entry_attr_timeout(&outentry), 0);
	if (!inode) {
		flags &= ~(O_CREAT | O_EXCL | O_TRUNC);
		fuse_sync_release(NULL, ff, flags);
		fuse_queue_forget(fm->fc, forget, outentry.nodeid, 1);
		err = -ENOMEM;
		goto out_err;
	}
	kfree(forget);
	d_instantiate(entry, inode);
	fuse_change_entry_timeout(entry, &outentry);
	fuse_dir_changed(dir);
	err = finish_open(file, entry, generic_file_open);
	if (err) {
		fi = get_fuse_inode(inode);
		fuse_sync_release(fi, ff, flags);
	} else {
		file->private_data = ff;
		fuse_finish_open(inode, file);
	}
	return err;

out_free_ff:
	fuse_file_free(ff);
out_put_forget_req:
	kfree(forget);
out_err:
	return err;
}