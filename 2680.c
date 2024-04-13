conninfo_getval(PQconninfoOption *connOptions,
				const char *keyword)
{
	PQconninfoOption *option;

	option = conninfo_find(connOptions, keyword);

	return option ? option->val : NULL;
}