static void ep_nested_calls_init(struct nested_calls *ncalls)
{
	INIT_LIST_HEAD(&ncalls->tasks_call_list);
	spin_lock_init(&ncalls->lock);
}