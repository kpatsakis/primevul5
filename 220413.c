static ssize_t tty_read(struct file *file, char __user *buf, size_t count,
			loff_t *ppos)
{
	int i;
	struct inode *inode = file_inode(file);
	struct tty_struct *tty = file_tty(file);
	struct tty_ldisc *ld;

	if (tty_paranoia_check(tty, inode, "tty_read"))
		return -EIO;
	if (!tty || tty_io_error(tty))
		return -EIO;

	/* We want to wait for the line discipline to sort out in this
	   situation */
	ld = tty_ldisc_ref_wait(tty);
	if (!ld)
		return hung_up_tty_read(file, buf, count, ppos);
	if (ld->ops->read)
		i = ld->ops->read(tty, file, buf, count);
	else
		i = -EIO;
	tty_ldisc_deref(ld);

	if (i > 0)
		tty_update_time(&inode->i_atime);

	return i;
}