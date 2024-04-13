static ssize_t lp_read(struct file * file, char __user * buf,
		       size_t count, loff_t *ppos)
{
	DEFINE_WAIT(wait);
	unsigned int minor=iminor(file_inode(file));
	struct parport *port = lp_table[minor].dev->port;
	ssize_t retval = 0;
	char *kbuf = lp_table[minor].lp_buffer;
	int nonblock = ((file->f_flags & O_NONBLOCK) ||
			(LP_F(minor) & LP_ABORT));

	if (count > LP_BUFFER_SIZE)
		count = LP_BUFFER_SIZE;

	if (mutex_lock_interruptible(&lp_table[minor].port_mutex))
		return -EINTR;

	lp_claim_parport_or_block (&lp_table[minor]);

	parport_set_timeout (lp_table[minor].dev,
			     (nonblock ? PARPORT_INACTIVITY_O_NONBLOCK
			      : lp_table[minor].timeout));

	parport_negotiate (lp_table[minor].dev->port, IEEE1284_MODE_COMPAT);
	if (parport_negotiate (lp_table[minor].dev->port,
			       IEEE1284_MODE_NIBBLE)) {
		retval = -EIO;
		goto out;
	}

	while (retval == 0) {
		retval = parport_read (port, kbuf, count);

		if (retval > 0)
			break;

		if (nonblock) {
			retval = -EAGAIN;
			break;
		}

		/* Wait for data. */

		if (lp_table[minor].dev->port->irq == PARPORT_IRQ_NONE) {
			parport_negotiate (lp_table[minor].dev->port,
					   IEEE1284_MODE_COMPAT);
			lp_error (minor);
			if (parport_negotiate (lp_table[minor].dev->port,
					       IEEE1284_MODE_NIBBLE)) {
				retval = -EIO;
				goto out;
			}
		} else {
			prepare_to_wait(&lp_table[minor].waitq, &wait, TASK_INTERRUPTIBLE);
			schedule_timeout(LP_TIMEOUT_POLLED);
			finish_wait(&lp_table[minor].waitq, &wait);
		}

		if (signal_pending (current)) {
			retval = -ERESTARTSYS;
			break;
		}

		cond_resched ();
	}
	parport_negotiate (lp_table[minor].dev->port, IEEE1284_MODE_COMPAT);
 out:
	lp_release_parport (&lp_table[minor]);

	if (retval > 0 && copy_to_user (buf, kbuf, retval))
		retval = -EFAULT;

	mutex_unlock(&lp_table[minor].port_mutex);

	return retval;
}