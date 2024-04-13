sslVerifyProtocolVersion(const char *version)
{
	/*
	 * An empty string and a NULL value are considered valid as it is
	 * equivalent to ignoring the parameter.
	 */
	if (!version || strlen(version) == 0)
		return true;

	if (pg_strcasecmp(version, "TLSv1") == 0 ||
		pg_strcasecmp(version, "TLSv1.1") == 0 ||
		pg_strcasecmp(version, "TLSv1.2") == 0 ||
		pg_strcasecmp(version, "TLSv1.3") == 0)
		return true;

	/* anything else is wrong */
	return false;
}