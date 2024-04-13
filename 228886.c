lka_report_init(void)
{
	struct reporters	*tailq;
	size_t			 i;

	dict_init(&report_smtp_in);
	dict_init(&report_smtp_out);

	for (i = 0; i < nitems(smtp_events); ++i) {
		tailq = xcalloc(1, sizeof (struct reporters));
		TAILQ_INIT(tailq);
		dict_xset(&report_smtp_in, smtp_events[i].event, tailq);

		tailq = xcalloc(1, sizeof (struct reporters));
		TAILQ_INIT(tailq);
		dict_xset(&report_smtp_out, smtp_events[i].event, tailq);
	}
}