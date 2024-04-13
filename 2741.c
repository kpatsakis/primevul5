sslVerifyProtocolRange(const char *min, const char *max)
{
	Assert(sslVerifyProtocolVersion(min) &&
		   sslVerifyProtocolVersion(max));

	/* If at least one of the bounds is not set, the range is valid */
	if (min == NULL || max == NULL || strlen(min) == 0 || strlen(max) == 0)
		return true;

	/*
	 * If the minimum version is the lowest one we accept, then all options
	 * for the maximum are valid.
	 */
	if (pg_strcasecmp(min, "TLSv1") == 0)
		return true;

	/*
	 * The minimum bound is valid, and cannot be TLSv1, so using TLSv1 for the
	 * maximum is incorrect.
	 */
	if (pg_strcasecmp(max, "TLSv1") == 0)
		return false;

	/*
	 * At this point we know that we have a mix of TLSv1.1 through 1.3
	 * versions.
	 */
	if (pg_strcasecmp(min, max) > 0)
		return false;

	return true;
}