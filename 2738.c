PQconninfoFree(PQconninfoOption *connOptions)
{
	PQconninfoOption *option;

	if (connOptions == NULL)
		return;

	for (option = connOptions; option->keyword != NULL; option++)
	{
		if (option->val != NULL)
			free(option->val);
	}
	free(connOptions);
}