random_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	ssize_t n, retval = 0, count = 0;

	if (nbytes == 0)
		return 0;

	while (nbytes > 0) {
		n = nbytes;
		if (n > SEC_XFER_SIZE)
			n = SEC_XFER_SIZE;

		DEBUG_ENT("reading %d bits\n", n*8);

		n = extract_entropy_user(&blocking_pool, buf, n);

		DEBUG_ENT("read got %d bits (%d still needed)\n",
			  n*8, (nbytes-n)*8);

		if (n == 0) {
			if (file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				break;
			}

			DEBUG_ENT("sleeping?\n");

			wait_event_interruptible(random_read_wait,
				input_pool.entropy_count >=
						 random_read_wakeup_thresh);

			DEBUG_ENT("awake\n");

			if (signal_pending(current)) {
				retval = -ERESTARTSYS;
				break;
			}

			continue;
		}

		if (n < 0) {
			retval = n;
			break;
		}
		count += n;
		buf += n;
		nbytes -= n;
		break;		/* This break makes the device work */
				/* like a named pipe */
	}

	/*
	 * If we gave the user some bytes, update the access time.
	 */
	if (count)
		file_accessed(file);

	return (count ? count : retval);
}