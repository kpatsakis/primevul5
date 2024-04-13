static int save_selftest(struct tracer *type)
{
	struct trace_selftests *selftest;

	selftest = kmalloc(sizeof(*selftest), GFP_KERNEL);
	if (!selftest)
		return -ENOMEM;

	selftest->type = type;
	list_add(&selftest->list, &postponed_selftests);
	return 0;
}