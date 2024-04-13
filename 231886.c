static long lp_ioctl(struct file *file, unsigned int cmd,
			unsigned long arg)
{
	unsigned int minor;
	struct timeval par_timeout;
	int ret;

	minor = iminor(file_inode(file));
	mutex_lock(&lp_mutex);
	switch (cmd) {
	case LPSETTIMEOUT:
		if (copy_from_user(&par_timeout, (void __user *)arg,
					sizeof (struct timeval))) {
			ret = -EFAULT;
			break;
		}
		ret = lp_set_timeout(minor, &par_timeout);
		break;
	default:
		ret = lp_do_ioctl(minor, cmd, arg, (void __user *)arg);
		break;
	}
	mutex_unlock(&lp_mutex);

	return ret;
}