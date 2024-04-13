static void v4l_print_std(const void *arg, bool write_only)
{
	pr_cont("std=0x%08Lx\n", *(const long long unsigned *)arg);
}