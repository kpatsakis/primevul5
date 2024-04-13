mono_loader_unlock_if_inited (void)
{
	if (loader_lock_inited)
		mono_loader_unlock ();
}