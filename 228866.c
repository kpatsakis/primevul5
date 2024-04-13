lka_filter_proc_in_session(uint64_t reqid, const char *proc)
{
	struct filter_session	*fs;
	struct filter		*filter;
	size_t			 i;

	if ((fs = tree_get(&sessions, reqid)) == NULL)
		return 0;

	filter = dict_get(&filters, fs->filter_name);
	if (filter == NULL || (filter->proc == NULL && filter->chain == NULL))
		return 0;

	if (filter->proc)
		return strcmp(filter->proc, proc) == 0 ? 1 : 0;

	for (i = 0; i < filter->chain_size; i++)
		if (filter->chain[i]->proc &&
		    strcmp(filter->chain[i]->proc, proc) == 0)
			return 1;

	return 0;
}