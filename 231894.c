static ssize_t lp_write(struct file * file, const char __user * buf,
		        size_t count, loff_t *ppos)
{
	unsigned int minor = iminor(file_inode(file));
	struct parport *port = lp_table[minor].dev->port;
	char *kbuf = lp_table[minor].lp_buffer;
	ssize_t retv = 0;
	ssize_t written;
	size_t copy_size = count;
	int nonblock = ((file->f_flags & O_NONBLOCK) ||
			(LP_F(minor) & LP_ABORT));

#ifdef LP_STATS
	if (time_after(jiffies, lp_table[minor].lastcall + LP_TIME(minor)))
		lp_table[minor].runchars = 0;

	lp_table[minor].lastcall = jiffies;
#endif

	/* Need to copy the data from user-space. */
	if (copy_size > LP_BUFFER_SIZE)
		copy_size = LP_BUFFER_SIZE;

	if (mutex_lock_interruptible(&lp_table[minor].port_mutex))
		return -EINTR;

	if (copy_from_user (kbuf, buf, copy_size)) {
		retv = -EFAULT;
		goto out_unlock;
	}

 	/* Claim Parport or sleep until it becomes available
 	 */
	lp_claim_parport_or_block (&lp_table[minor]);
	/* Go to the proper mode. */
	lp_table[minor].current_mode = lp_negotiate (port, 
						     lp_table[minor].best_mode);

	parport_set_timeout (lp_table[minor].dev,
			     (nonblock ? PARPORT_INACTIVITY_O_NONBLOCK
			      : lp_table[minor].timeout));

	if ((retv = lp_wait_ready (minor, nonblock)) == 0)
	do {
		/* Write the data. */
		written = parport_write (port, kbuf, copy_size);
		if (written > 0) {
			copy_size -= written;
			count -= written;
			buf  += written;
			retv += written;
		}

		if (signal_pending (current)) {
			if (retv == 0)
				retv = -EINTR;

			break;
		}

		if (copy_size > 0) {
			/* incomplete write -> check error ! */
			int error;

			parport_negotiate (lp_table[minor].dev->port, 
					   IEEE1284_MODE_COMPAT);
			lp_table[minor].current_mode = IEEE1284_MODE_COMPAT;

			error = lp_wait_ready (minor, nonblock);

			if (error) {
				if (retv == 0)
					retv = error;
				break;
			} else if (nonblock) {
				if (retv == 0)
					retv = -EAGAIN;
				break;
			}

			parport_yield_blocking (lp_table[minor].dev);
			lp_table[minor].current_mode 
			  = lp_negotiate (port, 
					  lp_table[minor].best_mode);

		} else if (need_resched())
			schedule ();

		if (count) {
			copy_size = count;
			if (copy_size > LP_BUFFER_SIZE)
				copy_size = LP_BUFFER_SIZE;

			if (copy_from_user(kbuf, buf, copy_size)) {
				if (retv == 0)
					retv = -EFAULT;
				break;
			}
		}	
	} while (count > 0);

	if (test_and_clear_bit(LP_PREEMPT_REQUEST, 
			       &lp_table[minor].bits)) {
		printk(KERN_INFO "lp%d releasing parport\n", minor);
		parport_negotiate (lp_table[minor].dev->port, 
				   IEEE1284_MODE_COMPAT);
		lp_table[minor].current_mode = IEEE1284_MODE_COMPAT;
		lp_release_parport (&lp_table[minor]);
	}
out_unlock:
	mutex_unlock(&lp_table[minor].port_mutex);

 	return retv;
}