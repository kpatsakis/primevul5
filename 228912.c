lka_proc_ready(void)
{
	void	*iter;
	struct processor_instance	*pi;

	iter = NULL;
	while (dict_iter(&processors, &iter, NULL, (void **)&pi))
		if (!pi->ready)
			return 0;
	return 1;
}