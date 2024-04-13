mono_loader_cleanup (void)
{
	dllmap_cleanup ();

	TlsFree (loader_error_thread_id);
	TlsFree (loader_lock_nest_id);

	DeleteCriticalSection (&loader_mutex);
	loader_lock_inited = FALSE;	
}