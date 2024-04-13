static int fuse_file_flock(struct file *file, int cmd, struct file_lock *fl)
{
	struct inode *inode = file->f_path.dentry->d_inode;
	struct fuse_conn *fc = get_fuse_conn(inode);
	int err;

	if (fc->no_lock) {
		err = flock_lock_file_wait(file, fl);
	} else {
		/* emulate flock with POSIX locks */
		fl->fl_owner = (fl_owner_t) file;
		err = fuse_setlk(file, fl, 1);
	}

	return err;
}