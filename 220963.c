cached_module_load (const char *name, int flags, char **err)
{
	MonoDl *res;

	if (err)
		*err = NULL;
	mono_loader_lock ();
	if (!global_module_map)
		global_module_map = g_hash_table_new (g_str_hash, g_str_equal);
	res = g_hash_table_lookup (global_module_map, name);
	if (res) {
		mono_loader_unlock ();
		return res;
	}
	res = mono_dl_open (name, flags, NULL);
	if (res)
		g_hash_table_insert (global_module_map, g_strdup (name), res);
	mono_loader_unlock ();
	return res;
}