mono_get_method_full (MonoImage *image, guint32 token, MonoClass *klass,
		      MonoGenericContext *context)
{
	MonoMethod *result;
	gboolean used_context = FALSE;

	/* We do everything inside the lock to prevent creation races */

	mono_image_lock (image);

	if (mono_metadata_token_table (token) == MONO_TABLE_METHOD) {
		if (!image->method_cache)
			image->method_cache = g_hash_table_new (NULL, NULL);
		result = g_hash_table_lookup (image->method_cache, GINT_TO_POINTER (mono_metadata_token_index (token)));
	} else {
		if (!image->methodref_cache)
			image->methodref_cache = g_hash_table_new (NULL, NULL);
		result = g_hash_table_lookup (image->methodref_cache, GINT_TO_POINTER (token));
	}
	mono_image_unlock (image);

	if (result)
		return result;

	result = mono_get_method_from_token (image, token, klass, context, &used_context);
	if (!result)
		return NULL;

	mono_image_lock (image);
	if (!used_context && !result->is_inflated) {
		MonoMethod *result2;
		if (mono_metadata_token_table (token) == MONO_TABLE_METHOD)
			result2 = g_hash_table_lookup (image->method_cache, GINT_TO_POINTER (mono_metadata_token_index (token)));
		else
			result2 = g_hash_table_lookup (image->methodref_cache, GINT_TO_POINTER (token));

		if (result2) {
			mono_image_unlock (image);
			return result2;
		}

		if (mono_metadata_token_table (token) == MONO_TABLE_METHOD)
			g_hash_table_insert (image->method_cache, GINT_TO_POINTER (mono_metadata_token_index (token)), result);
		else
			g_hash_table_insert (image->methodref_cache, GINT_TO_POINTER (token), result);
	}

	mono_image_unlock (image);

	return result;
}