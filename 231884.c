static int lp_check_status(int minor)
{
	int error = 0;
	unsigned int last = lp_table[minor].last_error;
	unsigned char status = r_str(minor);
	if ((status & LP_PERRORP) && !(LP_F(minor) & LP_CAREFUL))
		/* No error. */
		last = 0;
	else if ((status & LP_POUTPA)) {
		if (last != LP_POUTPA) {
			last = LP_POUTPA;
			printk(KERN_INFO "lp%d out of paper\n", minor);
		}
		error = -ENOSPC;
	} else if (!(status & LP_PSELECD)) {
		if (last != LP_PSELECD) {
			last = LP_PSELECD;
			printk(KERN_INFO "lp%d off-line\n", minor);
		}
		error = -EIO;
	} else if (!(status & LP_PERRORP)) {
		if (last != LP_PERRORP) {
			last = LP_PERRORP;
			printk(KERN_INFO "lp%d on fire\n", minor);
		}
		error = -EIO;
	} else {
		last = 0; /* Come here if LP_CAREFUL is set and no
                             errors are reported. */
	}

	lp_table[minor].last_error = last;

	if (last != 0)
		lp_error(minor);

	return error;
}