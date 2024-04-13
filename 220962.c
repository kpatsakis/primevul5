mono_dllmap_lookup_list (MonoDllMap *dll_map, const char *dll, const char* func, const char **rdll, const char **rfunc) {
	int found = 0;

	*rdll = dll;

	if (!dll_map)
		return 0;

	mono_loader_lock ();

	/* 
	 * we use the first entry we find that matches, since entries from
	 * the config file are prepended to the list and we document that the
	 * later entries win.
	 */
	for (; dll_map; dll_map = dll_map->next) {
		if (dll_map->dll [0] == 'i' && dll_map->dll [1] == ':') {
			if (g_ascii_strcasecmp (dll_map->dll + 2, dll))
				continue;
		} else if (strcmp (dll_map->dll, dll)) {
			continue;
		}
		if (!found && dll_map->target) {
			*rdll = dll_map->target;
			found = 1;
			/* we don't quit here, because we could find a full
			 * entry that matches also function and that has priority.
			 */
		}
		if (dll_map->func && strcmp (dll_map->func, func) == 0) {
			*rfunc = dll_map->target_func;
			break;
		}
	}

	mono_loader_unlock ();
	return found;
}