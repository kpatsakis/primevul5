static void t_stop(struct seq_file *m, void *p)
{
	mutex_unlock(&trace_types_lock);
}