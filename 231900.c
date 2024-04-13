static int lp_set_timeout(unsigned int minor, struct timeval *par_timeout)
{
	long to_jiffies;

	/* Convert to jiffies, place in lp_table */
	if ((par_timeout->tv_sec < 0) ||
	    (par_timeout->tv_usec < 0)) {
		return -EINVAL;
	}
	to_jiffies = DIV_ROUND_UP(par_timeout->tv_usec, 1000000/HZ);
	to_jiffies += par_timeout->tv_sec * (long) HZ;
	if (to_jiffies <= 0) {
		return -EINVAL;
	}
	lp_table[minor].timeout = to_jiffies;
	return 0;
}