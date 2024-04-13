mono_method_has_marshal_info (MonoMethod *method)
{
	int i, lastp;
	MonoClass *klass = method->klass;
	MonoTableInfo *methodt;
	MonoTableInfo *paramt;
	guint32 idx;

	if (method->klass->image->dynamic) {
		MonoReflectionMethodAux *method_aux = 
			g_hash_table_lookup (
				((MonoDynamicImage*)method->klass->image)->method_aux_hash, method);
		MonoMarshalSpec **dyn_specs = method_aux->param_marshall;
		if (dyn_specs) {
			for (i = 0; i < mono_method_signature (method)->param_count + 1; ++i)
				if (dyn_specs [i])
					return TRUE;
		}
		return FALSE;
	}

	mono_class_init (klass);

	methodt = &klass->image->tables [MONO_TABLE_METHOD];
	paramt = &klass->image->tables [MONO_TABLE_PARAM];
	idx = mono_method_get_index (method);
	if (idx > 0) {
		guint32 cols [MONO_PARAM_SIZE];
		guint param_index = mono_metadata_decode_row_col (methodt, idx - 1, MONO_METHOD_PARAMLIST);

		if (idx + 1 < methodt->rows)
			lastp = mono_metadata_decode_row_col (methodt, idx, MONO_METHOD_PARAMLIST);
		else
			lastp = paramt->rows + 1;

		for (i = param_index; i < lastp; ++i) {
			mono_metadata_decode_row (paramt, i -1, cols, MONO_PARAM_SIZE);

			if (cols [MONO_PARAM_FLAGS] & PARAM_ATTRIBUTE_HAS_FIELD_MARSHAL)
				return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}