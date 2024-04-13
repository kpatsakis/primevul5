static void *input_handlers_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	union input_seq_state *state = (union input_seq_state *)&seq->private;

	state->pos = *pos + 1;
	return seq_list_next(v, &input_handler_list, pos);
}