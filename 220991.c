field_from_memberref (MonoImage *image, guint32 token, MonoClass **retklass,
		      MonoGenericContext *context)
{
	MonoClass *klass;
	MonoClassField *field;
	MonoTableInfo *tables = image->tables;
	MonoType *sig_type;
	guint32 cols[6];
	guint32 nindex, class, class_table;
	const char *fname;
	const char *ptr;
	guint32 idx = mono_metadata_token_index (token);

	mono_metadata_decode_row (&tables [MONO_TABLE_MEMBERREF], idx-1, cols, MONO_MEMBERREF_SIZE);
	nindex = cols [MONO_MEMBERREF_CLASS] >> MONO_MEMBERREF_PARENT_BITS;
	class = cols [MONO_MEMBERREF_CLASS] & MONO_MEMBERREF_PARENT_MASK;

	fname = mono_metadata_string_heap (image, cols [MONO_MEMBERREF_NAME]);

	if (!mono_verifier_verify_memberref_signature (image, cols [MONO_MEMBERREF_SIGNATURE], NULL)) {
		mono_loader_set_error_bad_image (g_strdup_printf ("Bad field signature class token 0x%08x field name %s token 0x%08x on image %s", class, fname, token, image->name));
		return NULL;
	}

	switch (class) {
	case MONO_MEMBERREF_PARENT_TYPEDEF:
		class_table = MONO_TOKEN_TYPE_DEF;
		klass = mono_class_get (image, MONO_TOKEN_TYPE_DEF | nindex);
		break;
	case MONO_MEMBERREF_PARENT_TYPEREF:
		class_table = MONO_TOKEN_TYPE_REF;
		klass = mono_class_from_typeref (image, MONO_TOKEN_TYPE_REF | nindex);
		break;
	case MONO_MEMBERREF_PARENT_TYPESPEC:
		class_table = MONO_TOKEN_TYPE_SPEC;
		klass = mono_class_get_full (image, MONO_TOKEN_TYPE_SPEC | nindex, context);
		break;
	default:
		/*FIXME this must set a loader error!*/
		g_warning ("field load from %x", class);
		return NULL;
	}

	if (!klass) {
		char *name = mono_class_name_from_token (image, class_table | nindex);
		g_warning ("Missing field %s in class %s (type token %d)", fname, name, class_table | nindex);
		mono_loader_set_error_type_load (name, image->assembly_name);
		g_free (name);
		return NULL;
	}

	ptr = mono_metadata_blob_heap (image, cols [MONO_MEMBERREF_SIGNATURE]);
	mono_metadata_decode_blob_size (ptr, &ptr);
	/* we may want to check the signature here... */

	if (*ptr++ != 0x6) {
		g_warning ("Bad field signature class token %08x field name %s token %08x", class, fname, token);
		mono_loader_set_error_field_load (klass, fname);
		return NULL;
	}
	/* FIXME: This needs a cache, especially for generic instances, since
	 * mono_metadata_parse_type () allocates everything from a mempool.
	 */
	sig_type = find_cached_memberref_sig (image, cols [MONO_MEMBERREF_SIGNATURE]);
	if (!sig_type) {
		sig_type = mono_metadata_parse_type (image, MONO_PARSE_TYPE, 0, ptr, &ptr);
		if (sig_type == NULL) {
			mono_loader_set_error_field_load (klass, fname);
			return NULL;
		}
		sig_type = cache_memberref_sig (image, cols [MONO_MEMBERREF_SIGNATURE], sig_type);
	}

	mono_class_init (klass); /*FIXME is this really necessary?*/
	if (retklass)
		*retklass = klass;
	field = mono_class_get_field_from_name_full (klass, fname, sig_type);

	if (!field)
		mono_loader_set_error_field_load (klass, fname);

	return field;
}