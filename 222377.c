static void *input_devices_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	return seq_list_next(v, &input_dev_list, pos);
}