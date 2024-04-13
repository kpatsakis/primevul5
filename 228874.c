lka_filter_register_hook(const char *name, const char *hook)
{
	struct dict		*subsystem;
	struct filter		*filter;
	const char	*filter_name;
	void		*iter;
	size_t	i;

	if (strncasecmp(hook, "smtp-in|", 8) == 0) {
		subsystem = &filter_smtp_in;
		hook += 8;
	}
	else
		fatalx("Invalid message direction: %s", hook);

	for (i = 0; i < nitems(filter_execs); i++)
		if (strcmp(hook, filter_execs[i].phase_name) == 0)
			break;
	if (i == nitems(filter_execs))
		fatalx("Unrecognized report name: %s", hook);

	iter = NULL;
	while (dict_iter(&filters, &iter, &filter_name, (void **)&filter))
		if (filter->proc && strcmp(name, filter->proc) == 0)
			filter->phases |= (1<<filter_execs[i].phase);
}