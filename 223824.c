srs_hash_check(srs_t *srs, char *hash, int nargs, ...)
{
	va_list	ap;
	char	*srshash;
	char	*tmp;
	int		 len;
	int		 i;

	len = strlen(hash);
	if (len < srs->hashmin)
		return SRS_EHASHTOOSHORT;
	if (len > srs->hashlength) {
		tmp = alloca(srs->hashlength + 1);
		strncpy(tmp, hash, srs->hashlength);
		tmp[srs->hashlength] = '\0';
		hash = tmp;
		len = srs->hashlength;
	}

	for (i = 0; i < srs->numsecrets; i++) {
		va_start(ap, nargs);
		srshash = alloca(srs->hashlength + 1);
		srs_hash_create_v(srs, i, srshash, nargs, ap);
		va_end(ap);
		if (strncasecmp(hash, srshash, len) == 0)
			return SRS_SUCCESS;
	}

	return SRS_EHASHINVALID;
}