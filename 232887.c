table_destroy(struct smtpd *conf, struct table *t)
{
	dict_xpop(conf->sc_tables_dict, t->t_name);
	free(t);
}