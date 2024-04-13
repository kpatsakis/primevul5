parse_connection_string(const char *connstr, PQExpBuffer errorMessage,
						bool use_defaults)
{
	/* Parse as URI if connection string matches URI prefix */
	if (uri_prefix_length(connstr) != 0)
		return conninfo_uri_parse(connstr, errorMessage, use_defaults);

	/* Parse as default otherwise */
	return conninfo_parse(connstr, errorMessage, use_defaults);
}