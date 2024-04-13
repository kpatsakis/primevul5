int tracepoint_printk_sysctl(struct ctl_table *table, int write,
			     void __user *buffer, size_t *lenp,
			     loff_t *ppos)
{
	int save_tracepoint_printk;
	int ret;

	mutex_lock(&tracepoint_printk_mutex);
	save_tracepoint_printk = tracepoint_printk;

	ret = proc_dointvec(table, write, buffer, lenp, ppos);

	/*
	 * This will force exiting early, as tracepoint_printk
	 * is always zero when tracepoint_printk_iter is not allocated
	 */
	if (!tracepoint_print_iter)
		tracepoint_printk = 0;

	if (save_tracepoint_printk == tracepoint_printk)
		goto out;

	if (tracepoint_printk)
		static_key_enable(&tracepoint_printk_key.key);
	else
		static_key_disable(&tracepoint_printk_key.key);

 out:
	mutex_unlock(&tracepoint_printk_mutex);

	return ret;
}