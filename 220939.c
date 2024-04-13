mono_method_get_param_names (MonoMethod *method, const char **names)
{
	int i, lastp;
	MonoClass *klass;
	MonoTableInfo *methodt;
	MonoTableInfo *paramt;
	MonoMethodSignature *signature;
	guint32 idx;

	if (method->is_inflated)
		method = ((MonoMethodInflated *) method)->declaring;

	signature = mono_method_signature (method);
	/*FIXME this check is somewhat redundant since the caller usally will have to get the signature to figure out the
	  number of arguments and allocate a properly sized array. */
	if (signature == NULL)
		return;

	if (!signature->param_count)
		return;

	for (i = 0; i < signature->param_count; ++i)
		names [i] = "";

	klass = method->klass;
	if (klass->rank)
		return;

	mono_class_init (klass);

	if (klass->image->dynamic) {
		MonoReflectionMethodAux *method_aux = 
			g_hash_table_lookup (
				((MonoDynamicImage*)method->klass->image)->method_aux_hash, method);
		if (method_aux && method_aux->param_names) {
			for (i = 0; i < mono_method_signature (method)->param_count; ++i)
				if (method_aux->param_names [i + 1])
					names [i] = method_aux->param_names [i + 1];
		}
		return;
	}

	methodt = &klass->image->tables [MONO_TABLE_METHOD];
	paramt = &klass->image->tables [MONO_TABLE_PARAM];
	idx = mono_method_get_index (method);
	if (idx > 0) {
		guint32 cols [MONO_PARAM_SIZE];
		guint param_index;

		param_index = mono_metadata_decode_row_col (methodt, idx - 1, MONO_METHOD_PARAMLIST);

		if (idx < methodt->rows)
			lastp = mono_metadata_decode_row_col (methodt, idx, MONO_METHOD_PARAMLIST);
		else
			lastp = paramt->rows + 1;
		for (i = param_index; i < lastp; ++i) {
			mono_metadata_decode_row (paramt, i -1, cols, MONO_PARAM_SIZE);
			if (cols [MONO_PARAM_SEQUENCE] && cols [MONO_PARAM_SEQUENCE] <= signature->param_count) /* skip return param spec and bounds check*/
				names [cols [MONO_PARAM_SEQUENCE] - 1] = mono_metadata_string_heap (klass->image, cols [MONO_PARAM_NAME]);
		}
	}
}