mono_loader_lock_if_inited (void)
{
	if (loader_lock_inited)
		mono_loader_lock ();
}