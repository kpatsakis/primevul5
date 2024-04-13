lka_filter_ready(void)
{
	struct filter  	*filter;
	struct filter  	*subfilter;
	const char	*filter_name;
	struct filter_entry	*filter_entry;
	struct filter_chain	*filter_chain;
	void		*iter;
	size_t		i;
	size_t		j;

	/* all filters are ready, actually build the filter chains */
	iter = NULL;
	while (dict_iter(&filters, &iter, &filter_name, (void **)&filter)) {
		filter_chain = xcalloc(1, sizeof *filter_chain);
		for (i = 0; i < nitems(filter_execs); i++)
			TAILQ_INIT(&filter_chain->chain[i]);
		dict_set(&filter_chains, filter_name, filter_chain);

		if (filter->chain) {
			for (i = 0; i < filter->chain_size; i++) {
				subfilter = filter->chain[i];
				for (j = 0; j < nitems(filter_execs); ++j) {
					if (subfilter->phases & (1<<j)) {
						filter_entry = xcalloc(1, sizeof *filter_entry);
						filter_entry->id = generate_uid();
						filter_entry->name = subfilter->name;
						TAILQ_INSERT_TAIL(&filter_chain->chain[j],
						    filter_entry, entries);
					}
				}
			}
			continue;
		}

		for (i = 0; i < nitems(filter_execs); ++i) {
			if (filter->phases & (1<<i)) {
				filter_entry = xcalloc(1, sizeof *filter_entry);
				filter_entry->id = generate_uid();
				filter_entry->name = filter_name;
				TAILQ_INSERT_TAIL(&filter_chain->chain[i],
				    filter_entry, entries);
			}
		}
	}
}