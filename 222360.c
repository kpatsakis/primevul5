static void *input_devices_seq_start(struct seq_file *seq, loff_t *pos)
{
	union input_seq_state *state = (union input_seq_state *)&seq->private;
	int error;

	/* We need to fit into seq->private pointer */
	BUILD_BUG_ON(sizeof(union input_seq_state) != sizeof(seq->private));

	error = mutex_lock_interruptible(&input_mutex);
	if (error) {
		state->mutex_acquired = false;
		return ERR_PTR(error);
	}

	state->mutex_acquired = true;

	return seq_list_start(&input_dev_list, *pos);
}