void umd_cleanup_helper(struct umd_info *info)
{
	fput(info->pipe_to_umh);
	fput(info->pipe_from_umh);
	put_pid(info->tgid);
	info->tgid = NULL;
}