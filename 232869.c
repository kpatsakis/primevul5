table_create(struct smtpd *conf, const char *backend, const char *name,
    const char *config)
{
	struct table		*t;
	struct table_backend	*tb;
	char			 path[LINE_MAX];
	size_t			 n;
	struct stat		 sb;

	if (name && table_find(conf, name))
		fatalx("table_create: table \"%s\" already defined", name);

	if ((tb = table_backend_lookup(backend)) == NULL) {
		if ((size_t)snprintf(path, sizeof(path), PATH_LIBEXEC"/table-%s",
			backend) >= sizeof(path)) {
			fatalx("table_create: path too long \""
			    PATH_LIBEXEC"/table-%s\"", backend);
		}
		if (stat(path, &sb) == 0) {
			tb = table_backend_lookup("proc");
			(void)strlcpy(path, backend, sizeof(path));
			if (config) {
				(void)strlcat(path, ":", sizeof(path));
				if (strlcat(path, config, sizeof(path))
				    >= sizeof(path))
					fatalx("table_create: config file path too long");
			}
			config = path;
		}
	}

	if (tb == NULL)
		fatalx("table_create: backend \"%s\" does not exist", backend);

	t = xcalloc(1, sizeof(*t));
	t->t_backend = tb;

	if (config) {
		if (strlcpy(t->t_config, config, sizeof t->t_config)
		    >= sizeof t->t_config)
			fatalx("table_create: table config \"%s\" too large",
			    t->t_config);
	}

	if (strcmp(tb->name, "static") != 0)
		t->t_type = T_DYNAMIC;

	if (name == NULL)
		(void)snprintf(t->t_name, sizeof(t->t_name), "<dynamic:%u>",
		    last_table_id++);
	else {
		n = strlcpy(t->t_name, name, sizeof(t->t_name));
		if (n >= sizeof(t->t_name))
			fatalx("table_create: table name too long");
	}

	dict_set(conf->sc_tables_dict, t->t_name, t);

	return (t);
}