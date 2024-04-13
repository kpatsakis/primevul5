mono_dllmap_insert (MonoImage *assembly, const char *dll, const char *func, const char *tdll, const char *tfunc)
{
	MonoDllMap *entry;

	mono_loader_init ();

	mono_loader_lock ();

	if (!assembly) {
		entry = g_malloc0 (sizeof (MonoDllMap));
		entry->dll = dll? g_strdup (dll): NULL;
		entry->target = tdll? g_strdup (tdll): NULL;
		entry->func = func? g_strdup (func): NULL;
		entry->target_func = tfunc? g_strdup (tfunc): NULL;
		entry->next = global_dll_map;
		global_dll_map = entry;
	} else {
		entry = mono_image_alloc0 (assembly, sizeof (MonoDllMap));
		entry->dll = dll? mono_image_strdup (assembly, dll): NULL;
		entry->target = tdll? mono_image_strdup (assembly, tdll): NULL;
		entry->func = func? mono_image_strdup (assembly, func): NULL;
		entry->target_func = tfunc? mono_image_strdup (assembly, tfunc): NULL;
		entry->next = assembly->dll_map;
		assembly->dll_map = entry;
	}

	mono_loader_unlock ();
}