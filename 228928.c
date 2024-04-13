lka_filter_init(void)
{
	void		*iter;
	const char	*name;
	struct filter  	*filter;
	struct filter_config	*filter_config;
	size_t		i;
	char		 buffer[LINE_MAX];	/* for traces */

	dict_init(&filters);
	dict_init(&filter_chains);

	/* first pass, allocate and init individual filters */
	iter = NULL;
	while (dict_iter(env->sc_filters_dict, &iter, &name, (void **)&filter_config)) {
		switch (filter_config->filter_type) {
		case FILTER_TYPE_BUILTIN:
			filter = xcalloc(1, sizeof(*filter));
			filter->name = name;
			filter->phases |= (1<<filter_config->phase);
			filter->config = filter_config;
			dict_set(&filters, name, filter);
			log_trace(TRACE_FILTERS, "filters init type=builtin, name=%s, hooks=%08x",
			    name, filter->phases);
			break;

		case FILTER_TYPE_PROC:
			filter = xcalloc(1, sizeof(*filter));
			filter->name = name;
			filter->proc = filter_config->proc;
			filter->config = filter_config;
			dict_set(&filters, name, filter);
			log_trace(TRACE_FILTERS, "filters init type=proc, name=%s, proc=%s",
			    name, filter_config->proc);
			break;

		case FILTER_TYPE_CHAIN:
			break;
		}
	}

	/* second pass, allocate and init filter chains but don't build yet */
	iter = NULL;
	while (dict_iter(env->sc_filters_dict, &iter, &name, (void **)&filter_config)) {
		switch (filter_config->filter_type) {
		case FILTER_TYPE_CHAIN:
			filter = xcalloc(1, sizeof(*filter));
			filter->name = name;
			filter->chain = xcalloc(filter_config->chain_size, sizeof(void **));
			filter->chain_size = filter_config->chain_size;
			filter->config = filter_config;

			buffer[0] = '\0';
			for (i = 0; i < filter->chain_size; ++i) {
				filter->chain[i] = dict_xget(&filters, filter_config->chain[i]);
				if (i)
					(void)strlcat(buffer, ", ", sizeof buffer);
				(void)strlcat(buffer, filter->chain[i]->name, sizeof buffer);
			}
			log_trace(TRACE_FILTERS, "filters init type=chain, name=%s { %s }", name, buffer);

			dict_set(&filters, name, filter);
			break;

		case FILTER_TYPE_BUILTIN:
		case FILTER_TYPE_PROC:
			break;
		}
	}
}