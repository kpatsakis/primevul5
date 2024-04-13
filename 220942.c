mono_method_get_signature_full (MonoMethod *method, MonoImage *image, guint32 token, MonoGenericContext *context)
{
	int table = mono_metadata_token_table (token);
	int idx = mono_metadata_token_index (token);
	int sig_idx;
	guint32 cols [MONO_MEMBERREF_SIZE];
	MonoMethodSignature *sig;
	const char *ptr;

	/* !table is for wrappers: we should really assign their own token to them */
	if (!table || table == MONO_TABLE_METHOD)
		return mono_method_signature (method);

	if (table == MONO_TABLE_METHODSPEC) {
		/* the verifier (do_invoke_method) will turn the NULL into a verifier error */
		if ((method->flags & METHOD_ATTRIBUTE_PINVOKE_IMPL) || !method->is_inflated)
			return NULL;

		return mono_method_signature (method);
	}

	if (method->klass->generic_class)
		return mono_method_signature (method);

	if (image->dynamic) {
		sig = mono_reflection_lookup_signature (image, method, token);
	} else {
		mono_metadata_decode_row (&image->tables [MONO_TABLE_MEMBERREF], idx-1, cols, MONO_MEMBERREF_SIZE);
		sig_idx = cols [MONO_MEMBERREF_SIGNATURE];

		sig = find_cached_memberref_sig (image, sig_idx);
		if (!sig) {
			if (!mono_verifier_verify_memberref_signature (image, sig_idx, NULL)) {
				guint32 class = cols [MONO_MEMBERREF_CLASS] & MONO_MEMBERREF_PARENT_MASK;
				const char *fname = mono_metadata_string_heap (image, cols [MONO_MEMBERREF_NAME]);

				mono_loader_set_error_bad_image (g_strdup_printf ("Bad method signature class token 0x%08x field name %s token 0x%08x on image %s", class, fname, token, image->name));
				return NULL;
			}

			ptr = mono_metadata_blob_heap (image, sig_idx);
			mono_metadata_decode_blob_size (ptr, &ptr);
			sig = mono_metadata_parse_method_signature (image, 0, ptr, NULL);
			if (!sig)
				return NULL;
			sig = cache_memberref_sig (image, sig_idx, sig);
		}
		/* FIXME: we probably should verify signature compat in the dynamic case too*/
		if (!mono_verifier_is_sig_compatible (image, method, sig)) {
			guint32 class = cols [MONO_MEMBERREF_CLASS] & MONO_MEMBERREF_PARENT_MASK;
			const char *fname = mono_metadata_string_heap (image, cols [MONO_MEMBERREF_NAME]);

			mono_loader_set_error_bad_image (g_strdup_printf ("Incompatible method signature class token 0x%08x field name %s token 0x%08x on image %s", class, fname, token, image->name));
			return NULL;
		}
	}


	if (context) {
		MonoError error;
		MonoMethodSignature *cached;

		/* This signature is not owned by a MonoMethod, so need to cache */
		sig = inflate_generic_signature_checked (image, sig, context, &error);
		if (!mono_error_ok (&error)) {/*XXX bubble up this and kill one use of loader errors */
			mono_loader_set_error_bad_image (g_strdup_printf ("Could not inflate signature %s", mono_error_get_message (&error)));
			mono_error_cleanup (&error);
			return NULL;
		}

		cached = mono_metadata_get_inflated_signature (sig, context);
		if (cached != sig)
			mono_metadata_free_inflated_signature (sig);
		else
			inflated_signatures_size += mono_metadata_signature_size (cached);
		sig = cached;
	}

	return sig;
}