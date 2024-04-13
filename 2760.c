conninfo_find(PQconninfoOption *connOptions, const char *keyword)
{
	PQconninfoOption *option;

	for (option = connOptions; option->keyword != NULL; option++)
	{
		if (strcmp(option->keyword, keyword) == 0)
			return option;
	}

	return NULL;
}