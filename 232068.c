
static int bfq_var_store(unsigned long *var, const char *page)
{
	unsigned long new_val;
	int ret = kstrtoul(page, 10, &new_val);

	if (ret)
		return ret;
	*var = new_val;
	return 0;