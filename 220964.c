set_loader_error (MonoLoaderError *error)
{
	TlsSetValue (loader_error_thread_id, error);
}