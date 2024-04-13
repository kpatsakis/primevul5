mono_method_get_last_managed (void)
{
	MonoMethod *m = NULL;
	stack_walk (last_managed, FALSE, &m);
	return m;
}