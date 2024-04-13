static int __init set_tracepoint_printk(char *str)
{
	if ((strcmp(str, "=0") != 0 && strcmp(str, "=off") != 0))
		tracepoint_printk = 1;
	return 1;
}