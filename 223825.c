srs_set_malloc(srs_malloc_t m, srs_realloc_t r, srs_free_t f)
{
	srs_f_malloc = m;
	srs_f_realloc = r;
	srs_f_free = f;
	return SRS_SUCCESS;
}