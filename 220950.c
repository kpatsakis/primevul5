mono_loader_set_error_method_load (const char *class_name, const char *member_name)
{
	MonoLoaderError *error;

	/* FIXME: Store the signature as well */
	if (mono_loader_get_last_error ())
		return;

	error = g_new0 (MonoLoaderError, 1);
	error->exception_type = MONO_EXCEPTION_MISSING_METHOD;
	error->class_name = g_strdup (class_name);
	error->member_name = member_name;

	set_loader_error (error);
}