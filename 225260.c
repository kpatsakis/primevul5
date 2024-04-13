static void evdev_pass_values(struct evdev_client *client,
			const struct input_value *vals, unsigned int count,
			ktime_t mono, ktime_t real)
{
	struct evdev *evdev = client->evdev;
	const struct input_value *v;
	struct input_event event;
	bool wakeup = false;

	event.time = ktime_to_timeval(client->clkid == CLOCK_MONOTONIC ?
				      mono : real);

	/* Interrupts are disabled, just acquire the lock. */
	spin_lock(&client->buffer_lock);

	for (v = vals; v != vals + count; v++) {
		event.type = v->type;
		event.code = v->code;
		event.value = v->value;
		__pass_event(client, &event);
		if (v->type == EV_SYN && v->code == SYN_REPORT)
			wakeup = true;
	}

	spin_unlock(&client->buffer_lock);

	if (wakeup)
		wake_up_interruptible(&evdev->wait);
}