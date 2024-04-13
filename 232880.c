table_open_all(struct smtpd *conf)
{
	struct table	*t;
	void		*iter;

	iter = NULL;
	while (dict_iter(conf->sc_tables_dict, &iter, NULL, (void **)&t))
		if (!table_open(t))
			fatalx("failed to open table %s", t->t_name);
}