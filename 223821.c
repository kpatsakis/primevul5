srs_init(srs_t *srs)
{
	memset(srs, 0, sizeof(srs_t));
	srs->secrets = NULL;
	srs->numsecrets = 0;
	srs->separator = '=';
	srs->maxage = 21;
	srs->hashlength = 4;
	srs->hashmin = srs->hashlength;
	srs->alwaysrewrite = FALSE;
}