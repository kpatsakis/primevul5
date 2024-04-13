mono_stack_walk (MonoStackWalk func, gpointer user_data)
{
	stack_walk (func, TRUE, user_data);
}