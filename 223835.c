srs_free(srs_t *srs)
{
	int	 i;
	for (i = 0; i < srs->numsecrets; i++) {
		memset(srs->secrets[i], 0, strlen(srs->secrets[i]));
		srs_f_free(srs->secrets[i]);
		srs->secrets[i] = 0;
	}
	srs_f_free(srs);
}