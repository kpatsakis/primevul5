mono_method_get_param_token (MonoMethod *method, int index)
{
	MonoClass *klass = method->klass;
	MonoTableInfo *methodt;
	guint32 idx;

	mono_class_init (klass);

	if (klass->image->dynamic) {
		g_assert_not_reached ();
	}

	methodt = &klass->image->tables [MONO_TABLE_METHOD];
	idx = mono_method_get_index (method);
	if (idx > 0) {
		guint param_index = mono_metadata_decode_row_col (methodt, idx - 1, MONO_METHOD_PARAMLIST);

		if (index == -1)
			/* Return value */
			return mono_metadata_make_token (MONO_TABLE_PARAM, 0);
		else
			return mono_metadata_make_token (MONO_TABLE_PARAM, param_index + index);
	}

	return 0;
}