static int uvc_clock_param_set(const char *val, const struct kernel_param *kp)
{
	if (strncasecmp(val, "clock_", strlen("clock_")) == 0)
		val += strlen("clock_");

	if (strcasecmp(val, "monotonic") == 0)
		uvc_clock_param = CLOCK_MONOTONIC;
	else if (strcasecmp(val, "realtime") == 0)
		uvc_clock_param = CLOCK_REALTIME;
	else
		return -EINVAL;

	return 0;
}