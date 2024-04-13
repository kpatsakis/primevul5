static void __init apply_trace_boot_options(void)
{
	char *buf = trace_boot_options_buf;
	char *option;

	while (true) {
		option = strsep(&buf, ",");

		if (!option)
			break;

		if (*option)
			trace_set_options(&global_trace, option);

		/* Put back the comma to allow this to be called again */
		if (buf)
			*(buf - 1) = ',';
	}
}