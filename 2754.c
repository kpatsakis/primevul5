PQconndefaults(void)
{
	PQExpBufferData errorBuf;
	PQconninfoOption *connOptions;

	/* We don't actually report any errors here, but callees want a buffer */
	initPQExpBuffer(&errorBuf);
	if (PQExpBufferDataBroken(errorBuf))
		return NULL;			/* out of memory already :-( */

	connOptions = conninfo_init(&errorBuf);
	if (connOptions != NULL)
	{
		/* pass NULL errorBuf to ignore errors */
		if (!conninfo_add_defaults(connOptions, NULL))
		{
			PQconninfoFree(connOptions);
			connOptions = NULL;
		}
	}

	termPQExpBuffer(&errorBuf);
	return connOptions;
}