conninfo_init(PQExpBuffer errorMessage)
{
	PQconninfoOption *options;
	PQconninfoOption *opt_dest;
	const internalPQconninfoOption *cur_opt;

	/*
	 * Get enough memory for all options in PQconninfoOptions, even if some
	 * end up being filtered out.
	 */
	options = (PQconninfoOption *) malloc(sizeof(PQconninfoOption) * sizeof(PQconninfoOptions) / sizeof(PQconninfoOptions[0]));
	if (options == NULL)
	{
		appendPQExpBufferStr(errorMessage,
							 libpq_gettext("out of memory\n"));
		return NULL;
	}
	opt_dest = options;

	for (cur_opt = PQconninfoOptions; cur_opt->keyword; cur_opt++)
	{
		/* Only copy the public part of the struct, not the full internal */
		memcpy(opt_dest, cur_opt, sizeof(PQconninfoOption));
		opt_dest++;
	}
	MemSet(opt_dest, 0, sizeof(PQconninfoOption));

	return options;
}