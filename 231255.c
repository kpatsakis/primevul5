static int __init set_tracing_thresh(char *str)
{
	unsigned long threshold;
	int ret;

	if (!str)
		return 0;
	ret = kstrtoul(str, 0, &threshold);
	if (ret < 0)
		return 0;
	tracing_thresh = threshold * 1000;
	return 1;
}