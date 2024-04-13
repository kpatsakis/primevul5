srs_get_secret(srs_t *srs, int idx)
{
	if (idx < srs->numsecrets)
		return srs->secrets[idx];
	return NULL;
}