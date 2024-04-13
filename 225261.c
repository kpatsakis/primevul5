static unsigned int evdev_poll(struct file *file, poll_table *wait)
{
	struct evdev_client *client = file->private_data;
	struct evdev *evdev = client->evdev;
	unsigned int mask;

	poll_wait(file, &evdev->wait, wait);

	mask = evdev->exist ? POLLOUT | POLLWRNORM : POLLHUP | POLLERR;
	if (client->packet_head != client->tail)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}