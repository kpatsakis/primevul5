mono_stack_walk_no_il (MonoStackWalk func, gpointer user_data)
{
	stack_walk (func, FALSE, user_data);
}