srs_hash_create(srs_t *srs, char *buf, int nargs, ...)
{
	va_list	 ap;

	if (srs->numsecrets == 0)
		return SRS_ENOSECRETS;
	if (srs->secrets == NULL)
		return SRS_ENOSECRETS;
	if (srs->secrets[0] == NULL)
		return SRS_ENOSECRETS;

	va_start(ap, nargs);
	srs_hash_create_v(srs, 0, buf, nargs, ap);
	va_end(ap);

	return SRS_SUCCESS;
}