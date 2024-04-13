asmlinkage long sys_gethostname(char __user *name, int len)
{
	int i, errno;

	if (len < 0)
		return -EINVAL;
	down_read(&uts_sem);
	i = 1 + strlen(utsname()->nodename);
	if (i > len)
		i = len;
	errno = 0;
	if (copy_to_user(name, utsname()->nodename, i))
		errno = -EFAULT;
	up_read(&uts_sem);
	return errno;
}