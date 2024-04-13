srs_add_secret(srs_t *srs, const char *secret)
{
	int		newlen = (srs->numsecrets + 1) * sizeof(char *);
	srs->secrets = (char **)srs_f_realloc(srs->secrets, newlen);
	srs->secrets[srs->numsecrets++] = strdup(secret);
	return SRS_SUCCESS;
}