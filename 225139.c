static void v4l_print_buftype(const void *arg, bool write_only)
{
	pr_cont("type=%s\n", prt_names(*(u32 *)arg, v4l2_type_names));
}