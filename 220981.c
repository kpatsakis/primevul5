find_cached_memberref_sig (MonoImage *image, guint32 sig_idx)
{
	gpointer res;

	mono_loader_lock ();
	res = g_hash_table_lookup (image->memberref_signatures, GUINT_TO_POINTER (sig_idx));
	mono_loader_unlock ();

	return res;
}