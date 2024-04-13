static int uvc_clock_param_get(char *buffer, const struct kernel_param *kp)
{
	if (uvc_clock_param == CLOCK_MONOTONIC)
		return sprintf(buffer, "CLOCK_MONOTONIC");
	else
		return sprintf(buffer, "CLOCK_REALTIME");
}