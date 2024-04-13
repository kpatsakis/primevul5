static int __init stop_trace_on_warning(char *str)
{
	if ((strcmp(str, "=0") != 0 && strcmp(str, "=off") != 0))
		__disable_trace_on_warning = 1;
	return 1;
}