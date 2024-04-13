cache_memberref_sig (MonoImage *image, guint32 sig_idx, gpointer sig)
{
	gpointer prev_sig;

	mono_loader_lock ();
	prev_sig = g_hash_table_lookup (image->memberref_signatures, GUINT_TO_POINTER (sig_idx));
	if (prev_sig) {
		/* Somebody got in before us */
		sig = prev_sig;
	}
	else {
		g_hash_table_insert (image->memberref_signatures, GUINT_TO_POINTER (sig_idx), sig);
		/* An approximation based on glib 2.18 */
		memberref_sig_cache_size += sizeof (gpointer) * 4;
	}

	mono_loader_unlock ();

	return sig;
}