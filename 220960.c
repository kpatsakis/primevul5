mono_loader_get_last_error (void)
{
	return (MonoLoaderError*)TlsGetValue (loader_error_thread_id);
}