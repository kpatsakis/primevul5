mono_method_get_index (MonoMethod *method) {
	MonoClass *klass = method->klass;
	int i;

	if (klass->rank)
		/* constructed array methods are not in the MethodDef table */
		return 0;

	if (method->token)
		return mono_metadata_token_index (method->token);

	mono_class_setup_methods (klass);
	if (klass->exception_type)
		return 0;
	for (i = 0; i < klass->method.count; ++i) {
		if (method == klass->methods [i]) {
			if (klass->image->uncompressed_metadata)
				return mono_metadata_translate_token_index (klass->image, MONO_TABLE_METHOD, klass->method.first + i + 1);
			else
				return klass->method.first + i + 1;
		}
	}
	return 0;
}