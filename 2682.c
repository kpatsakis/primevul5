parse_int_param(const char *value, int *result, PGconn *conn,
				const char *context)
{
	char	   *end;
	long		numval;

	Assert(value != NULL);

	*result = 0;

	/* strtol(3) skips leading whitespaces */
	errno = 0;
	numval = strtol(value, &end, 10);

	/*
	 * If no progress was done during the parsing or an error happened, fail.
	 * This tests properly for overflows of the result.
	 */
	if (value == end || errno != 0 || numval != (int) numval)
		goto error;

	/*
	 * Skip any trailing whitespace; if anything but whitespace remains before
	 * the terminating character, fail
	 */
	while (*end != '\0' && isspace((unsigned char) *end))
		end++;

	if (*end != '\0')
		goto error;

	*result = numval;
	return true;

error:
	appendPQExpBuffer(&conn->errorMessage,
					  libpq_gettext("invalid integer value \"%s\" for connection option \"%s\"\n"),
					  value, context);
	return false;
}