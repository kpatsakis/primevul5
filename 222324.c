static __poll_t input_proc_devices_poll(struct file *file, poll_table *wait)
{
	poll_wait(file, &input_devices_poll_wait, wait);
	if (file->f_version != input_devices_state) {
		file->f_version = input_devices_state;
		return EPOLLIN | EPOLLRDNORM;
	}

	return 0;
}