srs_forward(srs_t *srs, char *buf, unsigned buflen,
				const char *sender, const char *alias)
{
	char	*senduser;
	char	*sendhost;
	char	*tmp;
	unsigned		 len;

	if (srs->noforward)
		return SRS_ENOTREWRITTEN;

	/* This is allowed to be a plain domain */
	while ((tmp = strchr(alias, '@')) != NULL)
		alias = tmp + 1;

	tmp = strchr(sender, '@');
	if (tmp == NULL)
		return SRS_ENOSENDERATSIGN;
	sendhost = tmp + 1;

	len = strlen(sender);

	if (! srs->alwaysrewrite) {
		if (strcasecmp(sendhost, alias) == 0) {
			if (strlen(sender) >= buflen)
				return SRS_EBUFTOOSMALL;
			strcpy(buf, sender);
			return SRS_SUCCESS;
		}
	}

	/* Reconstruct the whole show into our alloca() buffer. */
	senduser = alloca(len + 1);
	strcpy(senduser, sender);
	tmp = (senduser + (tmp - sender));
	sendhost = tmp + 1;
	*tmp = '\0';

	return srs_compile_guarded(srs, buf, buflen,
					sendhost, senduser, alias);
}