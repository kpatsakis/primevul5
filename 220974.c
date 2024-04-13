method_from_memberref (MonoImage *image, guint32 idx, MonoGenericContext *typespec_context,
		       gboolean *used_context)
{
	MonoClass *klass = NULL;
	MonoMethod *method = NULL;
	MonoTableInfo *tables = image->tables;
	guint32 cols[6];
	guint32 nindex, class, sig_idx;
	const char *mname;
	MonoMethodSignature *sig;
	const char *ptr;

	mono_metadata_decode_row (&tables [MONO_TABLE_MEMBERREF], idx-1, cols, 3);
	nindex = cols [MONO_MEMBERREF_CLASS] >> MONO_MEMBERREF_PARENT_BITS;
	class = cols [MONO_MEMBERREF_CLASS] & MONO_MEMBERREF_PARENT_MASK;
	/*g_print ("methodref: 0x%x 0x%x %s\n", class, nindex,
		mono_metadata_string_heap (m, cols [MONO_MEMBERREF_NAME]));*/

	mname = mono_metadata_string_heap (image, cols [MONO_MEMBERREF_NAME]);

	/*
	 * Whether we actually used the `typespec_context' or not.
	 * This is used to tell our caller whether or not it's safe to insert the returned
	 * method into a cache.
	 */
	if (used_context)
		*used_context = class == MONO_MEMBERREF_PARENT_TYPESPEC;

	switch (class) {
	case MONO_MEMBERREF_PARENT_TYPEREF:
		klass = mono_class_from_typeref (image, MONO_TOKEN_TYPE_REF | nindex);
		if (!klass) {
			char *name = mono_class_name_from_token (image, MONO_TOKEN_TYPE_REF | nindex);
			g_warning ("Missing method %s in assembly %s, type %s", mname, image->name, name);
			mono_loader_set_error_type_load (name, image->assembly_name);
			g_free (name);
			return NULL;
		}
		break;
	case MONO_MEMBERREF_PARENT_TYPESPEC:
		/*
		 * Parse the TYPESPEC in the parent's context.
		 */
		klass = mono_class_get_full (image, MONO_TOKEN_TYPE_SPEC | nindex, typespec_context);
		if (!klass) {
			char *name = mono_class_name_from_token (image, MONO_TOKEN_TYPE_SPEC | nindex);
			g_warning ("Missing method %s in assembly %s, type %s", mname, image->name, name);
			mono_loader_set_error_type_load (name, image->assembly_name);
			g_free (name);
			return NULL;
		}
		break;
	case MONO_MEMBERREF_PARENT_TYPEDEF:
		klass = mono_class_get (image, MONO_TOKEN_TYPE_DEF | nindex);
		if (!klass) {
			char *name = mono_class_name_from_token (image, MONO_TOKEN_TYPE_DEF | nindex);
			g_warning ("Missing method %s in assembly %s, type %s", mname, image->name, name);
			mono_loader_set_error_type_load (name, image->assembly_name);
			g_free (name);
			return NULL;
		}
		break;
	case MONO_MEMBERREF_PARENT_METHODDEF:
		return mono_get_method (image, MONO_TOKEN_METHOD_DEF | nindex, NULL);
		
	default:
		{
			/* This message leaks */
			char *message = g_strdup_printf ("Memberref parent unknown: class: %d, index %d", class, nindex);
			mono_loader_set_error_method_load ("", message);
			return NULL;
		}

	}
	g_assert (klass);
	mono_class_init (klass);

	sig_idx = cols [MONO_MEMBERREF_SIGNATURE];

	if (!mono_verifier_verify_memberref_signature (image, sig_idx, NULL)) {
		mono_loader_set_error_method_load (klass->name, mname);
		return NULL;
	}

	ptr = mono_metadata_blob_heap (image, sig_idx);
	mono_metadata_decode_blob_size (ptr, &ptr);

	sig = find_cached_memberref_sig (image, sig_idx);
	if (!sig) {
		sig = mono_metadata_parse_method_signature (image, 0, ptr, NULL);
		if (sig == NULL)
			return NULL;

		sig = cache_memberref_sig (image, sig_idx, sig);
	}

	switch (class) {
	case MONO_MEMBERREF_PARENT_TYPEREF:
	case MONO_MEMBERREF_PARENT_TYPEDEF:
		method = find_method (klass, NULL, mname, sig, klass);
		break;

	case MONO_MEMBERREF_PARENT_TYPESPEC: {
		MonoType *type;

		type = &klass->byval_arg;

		if (type->type != MONO_TYPE_ARRAY && type->type != MONO_TYPE_SZARRAY) {
			MonoClass *in_class = klass->generic_class ? klass->generic_class->container_class : klass;
			method = find_method (in_class, NULL, mname, sig, klass);
			break;
		}

		/* we're an array and we created these methods already in klass in mono_class_init () */
		method = mono_method_search_in_array_class (klass, mname, sig);
		break;
	}
	default:
		g_error ("Memberref parent unknown: class: %d, index %d", class, nindex);
		g_assert_not_reached ();
	}

	if (!method) {
		char *msig = mono_signature_get_desc (sig, FALSE);
		char * class_name = mono_type_get_name (&klass->byval_arg);
		GString *s = g_string_new (mname);
		if (sig->generic_param_count)
			g_string_append_printf (s, "<[%d]>", sig->generic_param_count);
		g_string_append_printf (s, "(%s)", msig);
		g_free (msig);
		msig = g_string_free (s, FALSE);

		g_warning (
			"Missing method %s::%s in assembly %s, referenced in assembly %s",
			class_name, msig, klass->image->name, image->name);
		mono_loader_set_error_method_load (class_name, mname);
		g_free (msig);
		g_free (class_name);
	}

	return method;
}