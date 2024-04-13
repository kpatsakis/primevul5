mono_loader_init ()
{
	static gboolean inited;

	if (!inited) {
		InitializeCriticalSection (&loader_mutex);
		loader_lock_inited = TRUE;

		loader_error_thread_id = TlsAlloc ();
		loader_lock_nest_id = TlsAlloc ();

		mono_counters_register ("Inflated signatures size",
								MONO_COUNTER_GENERICS | MONO_COUNTER_INT, &inflated_signatures_size);
		mono_counters_register ("Memberref signature cache size",
								MONO_COUNTER_METADATA | MONO_COUNTER_INT, &memberref_sig_cache_size);

		inited = TRUE;
	}
}