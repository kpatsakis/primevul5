srs_compile_shortcut(srs_t *srs,
				char *buf, int buflen,
				char *sendhost, char *senduser,
				const char *aliashost) {
	char	*srshash;
	char	 srsstamp[SRS_TIME_SIZE + 1];
	int		 len;
	int		 ret;

	/* This never happens if we get called from guarded() */
	if ((strncasecmp(senduser, SRS0TAG, 4) == 0) &&
		(strchr(srs_separators, senduser[4]) != NULL)) {
		sendhost = senduser + 5;
		if (*sendhost == '\0')
			return SRS_ENOSRS0HOST;
		senduser = strchr(sendhost, SRSSEP);
		if ((senduser == NULL) || (*senduser == '\0'))
			return SRS_ENOSRS0USER;
	}

	len = strlen(SRS0TAG) + 1 +
		srs->hashlength + 1 +
			SRS_TIME_SIZE + 1 +
				strlen(sendhost) + 1 + strlen(senduser)
			+ 1 + strlen(aliashost);
	if (len >= buflen)
		return SRS_EBUFTOOSMALL;

	ret = srs_timestamp_create(srs, srsstamp, time(NULL));
	if (ret != SRS_SUCCESS)
		return ret;
	srshash = alloca(srs->hashlength + 1);
	ret = srs_hash_create(srs, srshash,3, srsstamp, sendhost, senduser);
	if (ret != SRS_SUCCESS)
		return ret;

	sprintf(buf, SRS0TAG "%c%s%c%s%c%s%c%s@%s", srs->separator,
					srshash, SRSSEP, srsstamp, SRSSEP,
						sendhost, SRSSEP, senduser,
							aliashost);

	return SRS_SUCCESS;
}