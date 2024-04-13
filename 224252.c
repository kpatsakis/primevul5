static struct pid *pidfd_to_pid(const struct file *file)
{
	struct pid *pid;

	pid = pidfd_pid(file);
	if (!IS_ERR(pid))
		return pid;

	return tgid_pidfd_to_pid(file);
}