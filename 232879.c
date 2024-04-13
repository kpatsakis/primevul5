table_lookup(struct table *table, enum table_service kind, const char *key,
    union lookup *lk)
{
	char lkey[1024], *buf = NULL;
	int r;

	r = -1;
	if (table->t_backend->lookup == NULL)
		errno = ENOTSUP;
	else if (!lowercase(lkey, key, sizeof lkey)) {
		log_warnx("warn: lookup key too long: %s", key);
		errno = EINVAL;
	}
	else
		r = table->t_backend->lookup(table, kind, lkey, lk ? &buf : NULL);

	if (r == 1) {
		log_trace(TRACE_LOOKUP, "lookup: %s \"%s\" as %s in table %s:%s -> %s%s%s",
		    lk ? "lookup" : "match",
		    key,
		    table_service_name(kind),
		    table->t_backend->name,
		    table->t_name,
		    lk ? "\"" : "",
		    lk ? buf : "true",
		    lk ? "\"" : "");
		if (buf)
			r = table_parse_lookup(kind, lkey, buf, lk);
	}
	else
		log_trace(TRACE_LOOKUP, "lookup: %s \"%s\" as %s in table %s:%s -> %s%s",
		    lk ? "lookup" : "match",
		    key,
		    table_service_name(kind),
		    table->t_backend->name,
		    table->t_name,
		    (r == -1) ? "error: " : (lk ? "none" : "false"),
		    (r == -1) ? strerror(errno) : "");

	free(buf);

	return (r);
}