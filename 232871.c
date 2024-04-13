table_fetch(struct table *table, enum table_service kind, union lookup *lk)
{
	char *buf = NULL;
	int r;

	r = -1;
	if (table->t_backend->fetch == NULL)
		errno = ENOTSUP;
	else
		r = table->t_backend->fetch(table, kind, &buf);

	if (r == 1) {
		log_trace(TRACE_LOOKUP, "lookup: fetch %s from table %s:%s -> \"%s\"",
		    table_service_name(kind),
		    table->t_backend->name,
		    table->t_name,
		    buf);
		r = table_parse_lookup(kind, NULL, buf, lk);
	}
	else
		log_trace(TRACE_LOOKUP, "lookup: fetch %s from table %s:%s -> %s%s",
		    table_service_name(kind),
		    table->t_backend->name,
		    table->t_name,
		    (r == -1) ? "error: " : "none",
		    (r == -1) ? strerror(errno) : "");

	free(buf);

	return (r);
}