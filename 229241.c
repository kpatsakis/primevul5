timestamp_good(time_t then, time_t now, time_t interval)
{
	if (((now >= then) && ((now - then) < interval)) ||
	    ((now < then) && ((then - now) < (2 * interval)))) {
		return PAM_SUCCESS;
	}
	return PAM_AUTH_ERR;
}