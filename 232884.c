table_add(struct table *t, const char *key, const char *val)
{
	if (t->t_backend->add == NULL)
		fatalx("table_add: cannot add to table");

	if (t->t_backend->add(t, key, val) == 0)
		log_warnx("warn: failed to add \"%s\" in table \"%s\"", key, t->t_name);
}