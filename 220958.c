mono_loader_set_error_type_load (const char *class_name, const char *assembly_name)
{
	MonoLoaderError *error;

	if (mono_loader_get_last_error ()) 
		return;

	error = g_new0 (MonoLoaderError, 1);
	error->exception_type = MONO_EXCEPTION_TYPE_LOAD;
	error->class_name = g_strdup (class_name);
	error->assembly_name = g_strdup (assembly_name);

	/* 
	 * This is not strictly needed, but some (most) of the loader code still
	 * can't deal with load errors, and this message is more helpful than an
	 * assert.
	 */
	mono_trace_warning (MONO_TRACE_TYPE, "The class %s could not be loaded, used in %s", class_name, assembly_name);

	set_loader_error (error);
}