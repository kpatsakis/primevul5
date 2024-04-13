mono_loader_set_error_assembly_load (const char *assembly_name, gboolean ref_only)
{
	MonoLoaderError *error;

	if (mono_loader_get_last_error ()) 
		return;

	error = g_new0 (MonoLoaderError, 1);
	error->exception_type = MONO_EXCEPTION_FILE_NOT_FOUND;
	error->assembly_name = g_strdup (assembly_name);
	error->ref_only = ref_only;

	/* 
	 * This is not strictly needed, but some (most) of the loader code still
	 * can't deal with load errors, and this message is more helpful than an
	 * assert.
	 */
	if (ref_only)
		g_warning ("Cannot resolve dependency to assembly '%s' because it has not been preloaded. When using the ReflectionOnly APIs, dependent assemblies must be pre-loaded or loaded on demand through the ReflectionOnlyAssemblyResolve event.", assembly_name);
	else
		g_warning ("Could not load file or assembly '%s' or one of its dependencies.", assembly_name);

	set_loader_error (error);
}