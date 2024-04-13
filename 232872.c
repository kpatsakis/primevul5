table_find(struct smtpd *conf, const char *name)
{
	return dict_get(conf->sc_tables_dict, name);
}