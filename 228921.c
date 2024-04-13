lka_report_register_hook(const char *name, const char *hook)
{
	struct dict	*subsystem;
	struct reporter_proc	*rp;
	struct reporters	*tailq;
	void *iter;
	size_t	i;

	if (strncmp(hook, "smtp-in|", 8) == 0) {
		subsystem = &report_smtp_in;
		hook += 8;
	}
	else if (strncmp(hook, "smtp-out|", 9) == 0) {
		subsystem = &report_smtp_out;
		hook += 9;
	}
	else
		fatalx("Invalid message direction: %s", hook);

	if (strcmp(hook, "*") == 0) {
		iter = NULL;
		while (dict_iter(subsystem, &iter, NULL, (void **)&tailq)) {
			rp = xcalloc(1, sizeof *rp);
			rp->name = xstrdup(name);
			TAILQ_INSERT_TAIL(tailq, rp, entries);
		}
		return;
	}

	for (i = 0; i < nitems(smtp_events); i++)
		if (strcmp(hook, smtp_events[i].event) == 0)
			break;
	if (i == nitems(smtp_events))
		fatalx("Unrecognized report name: %s", hook);

	tailq = dict_get(subsystem, hook);
	rp = xcalloc(1, sizeof *rp);
	rp->name = xstrdup(name);
	TAILQ_INSERT_TAIL(tailq, rp, entries);
}