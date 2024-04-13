table_backend_lookup(const char *backend)
{
	int i;

	if (!strcmp(backend, "file"))
		backend = "static";

	for (i = 0; backends[i]; i++)
		if (!strcmp(backends[i]->name, backend))
			return (backends[i]);

	return NULL;
}