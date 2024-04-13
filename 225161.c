static void v4l_print_u32(const void *arg, bool write_only)
{
	pr_cont("value=%u\n", *(const u32 *)arg);
}