void FAST_FUNC dealloc_bunzip(bunzip_data *bd)
{
	free(bd->dbuf);
	free(bd);
}
