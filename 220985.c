mono_loader_lock (void)
{
	mono_locks_acquire (&loader_mutex, LoaderLock);
	if (G_UNLIKELY (loader_lock_track_ownership)) {
		TlsSetValue (loader_lock_nest_id, GUINT_TO_POINTER (GPOINTER_TO_UINT (TlsGetValue (loader_lock_nest_id)) + 1));
	}
}