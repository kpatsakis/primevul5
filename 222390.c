int input_handler_for_each_handle(struct input_handler *handler, void *data,
				  int (*fn)(struct input_handle *, void *))
{
	struct input_handle *handle;
	int retval = 0;

	rcu_read_lock();

	list_for_each_entry_rcu(handle, &handler->h_list, h_node) {
		retval = fn(handle, data);
		if (retval)
			break;
	}

	rcu_read_unlock();

	return retval;
}