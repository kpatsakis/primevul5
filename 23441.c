asmlinkage long sys_newuname(struct new_utsname __user * name)
{
	int errno = 0;

	down_read(&uts_sem);
	if (copy_to_user(name, utsname(), sizeof *name))
		errno = -EFAULT;
	up_read(&uts_sem);
	return errno;
}