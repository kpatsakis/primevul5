dllmap_cleanup (void)
{
	free_dllmap (global_dll_map);
	global_dll_map = NULL;
}