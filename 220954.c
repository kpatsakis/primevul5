mono_get_method_from_token (MonoImage *image, guint32 token, MonoClass *klass,
			    MonoGenericContext *context, gboolean *used_context)
{
	MonoMethod *result;
	int table = mono_metadata_token_table (token);
	int idx = mono_metadata_token_index (token);
	MonoTableInfo *tables = image->tables;
	MonoGenericContainer *generic_container = NULL, *container = NULL;
	const char *sig = NULL;
	int size;
	guint32 cols [MONO_TYPEDEF_SIZE];

	if (image->dynamic) {
		MonoClass *handle_class;

		result = mono_lookup_dynamic_token_class (image, token, TRUE, &handle_class, context);
		// This checks the memberref type as well
		if (result && handle_class != mono_defaults.methodhandle_class) {
			mono_loader_set_error_bad_image (g_strdup_printf ("Bad method token 0x%08x on image %s.", token, image->name));
			return NULL;
		}
		return result;
	}

	if (table != MONO_TABLE_METHOD) {
		if (table == MONO_TABLE_METHODSPEC) {
			if (used_context) *used_context = TRUE;
			return method_from_methodspec (image, context, idx);
		}
		if (table != MONO_TABLE_MEMBERREF) {
			g_warning ("got wrong token: 0x%08x\n", token);
			mono_loader_set_error_bad_image (g_strdup_printf ("Bad method token 0x%08x on image %s.", token, image->name));
			return NULL;
		}
		return method_from_memberref (image, idx, context, used_context);
	}

	if (used_context) *used_context = FALSE;

	if (idx > image->tables [MONO_TABLE_METHOD].rows) {
		mono_loader_set_error_bad_image (g_strdup_printf ("Bad method token 0x%08x on image %s.", token, image->name));
		return NULL;
	}

	mono_metadata_decode_row (&image->tables [MONO_TABLE_METHOD], idx - 1, cols, 6);

	if ((cols [2] & METHOD_ATTRIBUTE_PINVOKE_IMPL) ||
	    (cols [1] & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL))
		result = (MonoMethod *)mono_image_alloc0 (image, sizeof (MonoMethodPInvoke));
	else
		result = (MonoMethod *)mono_image_alloc0 (image, sizeof (MonoMethod));

	mono_stats.method_count ++;

	if (!klass) { /*FIXME put this before the image alloc*/
		guint32 type = mono_metadata_typedef_from_method (image, token);
		if (!type)
			return NULL;
		klass = mono_class_get (image, MONO_TOKEN_TYPE_DEF | type);
		if (klass == NULL)
			return NULL;
	}

	result->slot = -1;
	result->klass = klass;
	result->flags = cols [2];
	result->iflags = cols [1];
	result->token = token;
	result->name = mono_metadata_string_heap (image, cols [3]);

	if (!sig) /* already taken from the methodref */
		sig = mono_metadata_blob_heap (image, cols [4]);
	size = mono_metadata_decode_blob_size (sig, &sig);

	container = klass->generic_container;

	/* 
	 * load_generic_params does a binary search so only call it if the method 
	 * is generic.
	 */
	if (*sig & 0x10)
		generic_container = mono_metadata_load_generic_params (image, token, container);
	if (generic_container) {
		result->is_generic = TRUE;
		generic_container->owner.method = result;
		/*FIXME put this before the image alloc*/
		if (!mono_metadata_load_generic_param_constraints_full (image, token, generic_container))
			return NULL;

		container = generic_container;
	}

	if (cols [1] & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL) {
		if (result->klass == mono_defaults.string_class && !strcmp (result->name, ".ctor"))
			result->string_ctor = 1;
	} else if (cols [2] & METHOD_ATTRIBUTE_PINVOKE_IMPL) {
		MonoMethodPInvoke *piinfo = (MonoMethodPInvoke *)result;

#ifdef TARGET_WIN32
		/* IJW is P/Invoke with a predefined function pointer. */
		if (image->is_module_handle && (cols [1] & METHOD_IMPL_ATTRIBUTE_NATIVE)) {
			piinfo->addr = mono_image_rva_map (image, cols [0]);
			g_assert (piinfo->addr);
		}
#endif
		piinfo->implmap_idx = mono_metadata_implmap_from_method (image, idx - 1);
		/* Native methods can have no map. */
		if (piinfo->implmap_idx)
			piinfo->piflags = mono_metadata_decode_row_col (&tables [MONO_TABLE_IMPLMAP], piinfo->implmap_idx - 1, MONO_IMPLMAP_FLAGS);
	}

 	if (generic_container)
 		mono_method_set_generic_container (result, generic_container);

	return result;
}