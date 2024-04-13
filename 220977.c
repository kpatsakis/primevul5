mono_loader_set_error_field_load (MonoClass *klass, const char *member_name)
{
	MonoLoaderError *error;

	/* FIXME: Store the signature as well */
	if (mono_loader_get_last_error ())
		return;

	error = g_new0 (MonoLoaderError, 1);
	error->exception_type = MONO_EXCEPTION_MISSING_FIELD;
	error->klass = klass;
	error->member_name = member_name;

	set_loader_error (error);
}