static int lp_wait_ready(int minor, int nonblock)
{
	int error = 0;

	/* If we're not in compatibility mode, we're ready now! */
	if (lp_table[minor].current_mode != IEEE1284_MODE_COMPAT) {
	  return (0);
	}

	do {
		error = lp_check_status (minor);
		if (error && (nonblock || (LP_F(minor) & LP_ABORT)))
			break;
		if (signal_pending (current)) {
			error = -EINTR;
			break;
		}
	} while (error);
	return error;
}