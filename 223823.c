srs_reverse(srs_t *srs, char *buf, unsigned buflen, const char *sender)
{
	char	*senduser;
	char	*tmp;
	unsigned		 len;

	if (!SRS_IS_SRS_ADDRESS(sender))
		return SRS_ENOTSRSADDRESS;

	if (srs->noreverse)
		return SRS_ENOTREWRITTEN;

	len = strlen(sender);
	if (len >= buflen)
		return SRS_EBUFTOOSMALL;
	senduser = alloca(len + 1);
	strcpy(senduser, sender);

	/* We don't really care about the host for reversal. */
	tmp = strchr(senduser, '@');
	if (tmp != NULL)
		*tmp = '\0';
	return srs_parse_guarded(srs, buf, buflen, senduser);
}