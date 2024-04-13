mono_loader_clear_error (void)
{
	MonoLoaderError *ex = (MonoLoaderError*)TlsGetValue (loader_error_thread_id);

	if (ex) {
		g_free (ex->class_name);
		g_free (ex->assembly_name);
		g_free (ex->msg);
		g_free (ex);
	
		TlsSetValue (loader_error_thread_id, NULL);
	}
}