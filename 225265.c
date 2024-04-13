static void evdev_events(struct input_handle *handle,
			 const struct input_value *vals, unsigned int count)
{
	struct evdev *evdev = handle->private;
	struct evdev_client *client;
	ktime_t time_mono, time_real;

	time_mono = ktime_get();
	time_real = ktime_sub(time_mono, ktime_get_monotonic_offset());

	rcu_read_lock();

	client = rcu_dereference(evdev->grab);

	if (client)
		evdev_pass_values(client, vals, count, time_mono, time_real);
	else
		list_for_each_entry_rcu(client, &evdev->client_list, node)
			evdev_pass_values(client, vals, count,
					  time_mono, time_real);

	rcu_read_unlock();
}