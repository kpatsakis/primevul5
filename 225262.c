static void evdev_queue_syn_dropped(struct evdev_client *client)
{
	unsigned long flags;
	struct input_event ev;
	ktime_t time;

	time = ktime_get();
	if (client->clkid != CLOCK_MONOTONIC)
		time = ktime_sub(time, ktime_get_monotonic_offset());

	ev.time = ktime_to_timeval(time);
	ev.type = EV_SYN;
	ev.code = SYN_DROPPED;
	ev.value = 0;

	spin_lock_irqsave(&client->buffer_lock, flags);

	client->buffer[client->head++] = ev;
	client->head &= client->bufsize - 1;

	if (unlikely(client->head == client->tail)) {
		/* drop queue but keep our SYN_DROPPED event */
		client->tail = (client->head - 1) & (client->bufsize - 1);
		client->packet_head = client->tail;
	}

	spin_unlock_irqrestore(&client->buffer_lock, flags);
}