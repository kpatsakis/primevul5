static int umd_setup(struct subprocess_info *info, struct cred *new)
{
	struct umd_info *umd_info = info->data;
	struct file *from_umh[2];
	struct file *to_umh[2];
	int err;

	/* create pipe to send data to umh */
	err = create_pipe_files(to_umh, 0);
	if (err)
		return err;
	err = replace_fd(0, to_umh[0], 0);
	fput(to_umh[0]);
	if (err < 0) {
		fput(to_umh[1]);
		return err;
	}

	/* create pipe to receive data from umh */
	err = create_pipe_files(from_umh, 0);
	if (err) {
		fput(to_umh[1]);
		replace_fd(0, NULL, 0);
		return err;
	}
	err = replace_fd(1, from_umh[1], 0);
	fput(from_umh[1]);
	if (err < 0) {
		fput(to_umh[1]);
		replace_fd(0, NULL, 0);
		fput(from_umh[0]);
		return err;
	}

	set_fs_pwd(current->fs, &umd_info->wd);
	umd_info->pipe_to_umh = to_umh[1];
	umd_info->pipe_from_umh = from_umh[0];
	umd_info->tgid = get_pid(task_tgid(current));
	return 0;
}