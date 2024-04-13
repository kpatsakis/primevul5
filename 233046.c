static struct CRYPTO_dynlock_value *dyn_create_function(const char *file, int line)
{
	struct CRYPTO_dynlock_value *value;

	value = (struct CRYPTO_dynlock_value *)
		malloc(sizeof(struct CRYPTO_dynlock_value));
	if (!value)
		goto err;
	MUTEX_INIT(value->mutex);

	return value;

err:
	return (NULL);
}