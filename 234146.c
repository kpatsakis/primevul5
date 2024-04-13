random_poll(struct file *file, poll_table * wait)
{
	unsigned int mask;

	poll_wait(file, &random_read_wait, wait);
	poll_wait(file, &random_write_wait, wait);
	mask = 0;
	if (input_pool.entropy_count >= random_read_wakeup_thresh)
		mask |= POLLIN | POLLRDNORM;
	if (input_pool.entropy_count < random_write_wakeup_thresh)
		mask |= POLLOUT | POLLWRNORM;
	return mask;
}