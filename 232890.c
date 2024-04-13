table_close_all(struct smtpd *conf)
{
	struct table	*t;
	void		*iter;

	iter = NULL;
	while (dict_iter(conf->sc_tables_dict, &iter, NULL, (void **)&t))
		table_close(t);
}