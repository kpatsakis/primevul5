mono_method_search_in_array_class (MonoClass *klass, const char *name, MonoMethodSignature *sig)
{
	int i;

	mono_class_setup_methods (klass);
	g_assert (!klass->exception_type); /*FIXME this should not fail, right?*/
	for (i = 0; i < klass->method.count; ++i) {
		MonoMethod *method = klass->methods [i];
		if (strcmp (method->name, name) == 0 && sig->param_count == method->signature->param_count)
			return method;
	}
	return NULL;
}