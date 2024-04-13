static int __init set_ftrace_dump_on_oops(char *str)
{
	if (*str++ != '=' || !*str) {
		ftrace_dump_on_oops = DUMP_ALL;
		return 1;
	}

	if (!strcmp("orig_cpu", str)) {
		ftrace_dump_on_oops = DUMP_ORIG;
                return 1;
        }

        return 0;
}