static void saved_cmdlines_stop(struct seq_file *m, void *v)
{
	arch_spin_unlock(&trace_cmdline_lock);
	preempt_enable();
}