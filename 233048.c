dyn_destroy_function(struct CRYPTO_dynlock_value *l, const char *file, int line)
{
	MUTEX_FREE(l->mutex);
	free(l);
}