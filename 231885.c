static int lp_open(struct inode * inode, struct file * file)
{
	unsigned int minor = iminor(inode);
	int ret = 0;

	mutex_lock(&lp_mutex);
	if (minor >= LP_NO) {
		ret = -ENXIO;
		goto out;
	}
	if ((LP_F(minor) & LP_EXIST) == 0) {
		ret = -ENXIO;
		goto out;
	}
	if (test_and_set_bit(LP_BUSY_BIT_POS, &LP_F(minor))) {
		ret = -EBUSY;
		goto out;
	}
	/* If ABORTOPEN is set and the printer is offline or out of paper,
	   we may still want to open it to perform ioctl()s.  Therefore we
	   have commandeered O_NONBLOCK, even though it is being used in
	   a non-standard manner.  This is strictly a Linux hack, and
	   should most likely only ever be used by the tunelp application. */
	if ((LP_F(minor) & LP_ABORTOPEN) && !(file->f_flags & O_NONBLOCK)) {
		int status;
		lp_claim_parport_or_block (&lp_table[minor]);
		status = r_str(minor);
		lp_release_parport (&lp_table[minor]);
		if (status & LP_POUTPA) {
			printk(KERN_INFO "lp%d out of paper\n", minor);
			LP_F(minor) &= ~LP_BUSY;
			ret = -ENOSPC;
			goto out;
		} else if (!(status & LP_PSELECD)) {
			printk(KERN_INFO "lp%d off-line\n", minor);
			LP_F(minor) &= ~LP_BUSY;
			ret = -EIO;
			goto out;
		} else if (!(status & LP_PERRORP)) {
			printk(KERN_ERR "lp%d printer error\n", minor);
			LP_F(minor) &= ~LP_BUSY;
			ret = -EIO;
			goto out;
		}
	}
	lp_table[minor].lp_buffer = kmalloc(LP_BUFFER_SIZE, GFP_KERNEL);
	if (!lp_table[minor].lp_buffer) {
		LP_F(minor) &= ~LP_BUSY;
		ret = -ENOMEM;
		goto out;
	}
	/* Determine if the peripheral supports ECP mode */
	lp_claim_parport_or_block (&lp_table[minor]);
	if ( (lp_table[minor].dev->port->modes & PARPORT_MODE_ECP) &&
             !parport_negotiate (lp_table[minor].dev->port, 
                                 IEEE1284_MODE_ECP)) {
		printk (KERN_INFO "lp%d: ECP mode\n", minor);
		lp_table[minor].best_mode = IEEE1284_MODE_ECP;
	} else {
		lp_table[minor].best_mode = IEEE1284_MODE_COMPAT;
	}
	/* Leave peripheral in compatibility mode */
	parport_negotiate (lp_table[minor].dev->port, IEEE1284_MODE_COMPAT);
	lp_release_parport (&lp_table[minor]);
	lp_table[minor].current_mode = IEEE1284_MODE_COMPAT;
out:
	mutex_unlock(&lp_mutex);
	return ret;
}