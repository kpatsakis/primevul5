mono_loader_lock_is_owned_by_self (void)
{
	g_assert (loader_lock_track_ownership);

	return GPOINTER_TO_UINT (TlsGetValue (loader_lock_nest_id)) > 0;
}