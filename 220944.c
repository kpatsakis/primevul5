find_method_in_class (MonoClass *klass, const char *name, const char *qname, const char *fqname,
		      MonoMethodSignature *sig, MonoClass *from_class)
{
 	int i;

	/* Search directly in the metadata to avoid calling setup_methods () */

	/* FIXME: !from_class->generic_class condition causes test failures. */
	if (klass->type_token && !klass->image->dynamic && !klass->methods && !klass->rank && klass == from_class && !from_class->generic_class) {
		for (i = 0; i < klass->method.count; ++i) {
			guint32 cols [MONO_METHOD_SIZE];
			MonoMethod *method;
			const char *m_name;
			MonoMethodSignature *other_sig;

			mono_metadata_decode_table_row (klass->image, MONO_TABLE_METHOD, klass->method.first + i, cols, MONO_METHOD_SIZE);

			m_name = mono_metadata_string_heap (klass->image, cols [MONO_METHOD_NAME]);

			if (!((fqname && !strcmp (m_name, fqname)) ||
				  (qname && !strcmp (m_name, qname)) ||
				  (name && !strcmp (m_name, name))))
				continue;

			method = mono_get_method (klass->image, MONO_TOKEN_METHOD_DEF | (klass->method.first + i + 1), klass);
			other_sig = mono_method_signature (method);
			if (method && other_sig && (sig->call_convention != MONO_CALL_VARARG) && mono_metadata_signature_equal (sig, other_sig))
				return method;
		}
	}

	mono_class_setup_methods (klass);
	/*
	We can't fail lookup of methods otherwise the runtime will fail with MissingMethodException instead of TypeLoadException.
	See mono/tests/generic-type-load-exception.2.il
	FIXME we should better report this error to the caller
	 */
	if (!klass->methods)
		return NULL;
	for (i = 0; i < klass->method.count; ++i) {
		MonoMethod *m = klass->methods [i];
		MonoMethodSignature *msig;

		if (!((fqname && !strcmp (m->name, fqname)) ||
		      (qname && !strcmp (m->name, qname)) ||
		      (name && !strcmp (m->name, name))))
			continue;
		msig = mono_method_signature (m);
		if (!msig)
			continue;

		if (sig->call_convention == MONO_CALL_VARARG) {
			if (mono_metadata_signature_vararg_match (sig, msig))
				break;
		} else {
			if (mono_metadata_signature_equal (sig, msig))
				break;
		}
	}

	if (i < klass->method.count)
		return mono_class_get_method_by_index (from_class, i);
	return NULL;
}