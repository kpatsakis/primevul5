asmlinkage long sys_sethostname(char __user *name, int len)
{
	int errno;
	char tmp[__NEW_UTS_LEN];

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	if (len < 0 || len > __NEW_UTS_LEN)
		return -EINVAL;
	down_write(&uts_sem);
	errno = -EFAULT;
	if (!copy_from_user(tmp, name, len)) {
		memcpy(utsname()->nodename, tmp, len);
		utsname()->nodename[len] = 0;
		errno = 0;
	}
	up_write(&uts_sem);
	return errno;
}