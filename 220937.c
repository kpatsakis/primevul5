mono_field_from_token (MonoImage *image, guint32 token, MonoClass **retklass,
		       MonoGenericContext *context)
{
	MonoClass *k;
	guint32 type;
	MonoClassField *field;

	if (image->dynamic) {
		MonoClassField *result;
		MonoClass *handle_class;

		*retklass = NULL;
		result = mono_lookup_dynamic_token_class (image, token, TRUE, &handle_class, context);
		// This checks the memberref type as well
		if (!result || handle_class != mono_defaults.fieldhandle_class) {
			mono_loader_set_error_bad_image (g_strdup_printf ("Bad field token 0x%08x on image %s.", token, image->name));
			return NULL;
		}
		*retklass = result->parent;
		return result;
	}

	mono_loader_lock ();
	if ((field = g_hash_table_lookup (image->field_cache, GUINT_TO_POINTER (token)))) {
		*retklass = field->parent;
		mono_loader_unlock ();
		return field;
	}
	mono_loader_unlock ();

	if (mono_metadata_token_table (token) == MONO_TABLE_MEMBERREF)
		field = field_from_memberref (image, token, retklass, context);
	else {
		type = mono_metadata_typedef_from_field (image, mono_metadata_token_index (token));
		if (!type)
			return NULL;
		k = mono_class_get (image, MONO_TOKEN_TYPE_DEF | type);
		if (!k)
			return NULL;
		mono_class_init (k);
		if (retklass)
			*retklass = k;
		field = mono_class_get_field (k, token);
	}

	mono_loader_lock ();
	if (field && field->parent && !field->parent->generic_class && !field->parent->generic_container)
		g_hash_table_insert (image->field_cache, GUINT_TO_POINTER (token), field);
	mono_loader_unlock ();
	return field;
}