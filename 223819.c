srs_forward_alloc(srs_t *srs, char **sptr,
				const char *sender, const char *alias)
{
	char	*buf;
	int		 slen;
	int		 alen;
	int		 len;
	int		 ret;

	if (srs->noforward)
		return SRS_ENOTREWRITTEN;

	slen = strlen(sender);
	alen = strlen(alias);

	/* strlen(SRSxTAG) + strlen("====+@") < 64 */
	len = slen + alen + srs->hashlength + SRS_TIME_SIZE + 64;
	buf = (char *)srs_f_malloc(len);

	ret = srs_forward(srs, buf, len, sender, alias);

	if (ret == SRS_SUCCESS)
		*sptr = buf;
	else
		srs_f_free(buf);

	return ret;
}