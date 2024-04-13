static void input_seq_stop(struct seq_file *seq, void *v)
{
	union input_seq_state *state = (union input_seq_state *)&seq->private;

	if (state->mutex_acquired)
		mutex_unlock(&input_mutex);
}