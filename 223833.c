srs_reverse_alloc(srs_t *srs, char **sptr, const char *sender)
{
	char	*buf;
	int		 len;
	int		 ret;

	*sptr = NULL;

	if (!SRS_IS_SRS_ADDRESS(sender))
		return SRS_ENOTSRSADDRESS;

	if (srs->noreverse)
		return SRS_ENOTREWRITTEN;

	len = strlen(sender) + 1;
	buf = (char *)srs_f_malloc(len);

	ret = srs_reverse(srs, buf, len, sender);

	if (ret == SRS_SUCCESS)
		*sptr = buf;
	else
		srs_f_free(buf);

	return ret;
}