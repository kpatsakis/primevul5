method_from_methodspec (MonoImage *image, MonoGenericContext *context, guint32 idx)
{
	MonoError error;
	MonoMethod *method;
	MonoClass *klass;
	MonoTableInfo *tables = image->tables;
	MonoGenericContext new_context;
	MonoGenericInst *inst;
	const char *ptr;
	guint32 cols [MONO_METHODSPEC_SIZE];
	guint32 token, nindex, param_count;

	mono_metadata_decode_row (&tables [MONO_TABLE_METHODSPEC], idx - 1, cols, MONO_METHODSPEC_SIZE);
	token = cols [MONO_METHODSPEC_METHOD];
	nindex = token >> MONO_METHODDEFORREF_BITS;

	if (!mono_verifier_verify_methodspec_signature (image, cols [MONO_METHODSPEC_SIGNATURE], NULL))
		return NULL;

	ptr = mono_metadata_blob_heap (image, cols [MONO_METHODSPEC_SIGNATURE]);

	mono_metadata_decode_value (ptr, &ptr);
	ptr++;
	param_count = mono_metadata_decode_value (ptr, &ptr);
	g_assert (param_count);

	inst = mono_metadata_parse_generic_inst (image, NULL, param_count, ptr, &ptr);
	if (context && inst->is_open) {
		inst = mono_metadata_inflate_generic_inst (inst, context, &error);
		if (!mono_error_ok (&error)) {
			mono_error_cleanup (&error); /*FIXME don't swallow error message.*/
			return NULL;
		}
	}

	if ((token & MONO_METHODDEFORREF_MASK) == MONO_METHODDEFORREF_METHODDEF)
		method = mono_get_method_full (image, MONO_TOKEN_METHOD_DEF | nindex, NULL, context);
	else
		method = method_from_memberref (image, nindex, context, NULL);

	if (!method)
		return NULL;

	klass = method->klass;

	if (klass->generic_class) {
		g_assert (method->is_inflated);
		method = ((MonoMethodInflated *) method)->declaring;
	}

	new_context.class_inst = klass->generic_class ? klass->generic_class->context.class_inst : NULL;
	new_context.method_inst = inst;

	return mono_class_inflate_generic_method_full (method, klass, &new_context);
}