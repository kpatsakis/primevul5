srs_new()
{
	srs_t	*srs = (srs_t *)srs_f_malloc(sizeof(srs_t));
	srs_init(srs);
	return srs;
}