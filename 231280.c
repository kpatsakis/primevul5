tracing_cpumask_write(struct file *filp, const char __user *ubuf,
		      size_t count, loff_t *ppos)
{
	struct trace_array *tr = file_inode(filp)->i_private;
	cpumask_var_t tracing_cpumask_new;
	int err, cpu;

	if (!alloc_cpumask_var(&tracing_cpumask_new, GFP_KERNEL))
		return -ENOMEM;

	err = cpumask_parse_user(ubuf, count, tracing_cpumask_new);
	if (err)
		goto err_unlock;

	local_irq_disable();
	arch_spin_lock(&tr->max_lock);
	for_each_tracing_cpu(cpu) {
		/*
		 * Increase/decrease the disabled counter if we are
		 * about to flip a bit in the cpumask:
		 */
		if (cpumask_test_cpu(cpu, tr->tracing_cpumask) &&
				!cpumask_test_cpu(cpu, tracing_cpumask_new)) {
			atomic_inc(&per_cpu_ptr(tr->trace_buffer.data, cpu)->disabled);
			ring_buffer_record_disable_cpu(tr->trace_buffer.buffer, cpu);
		}
		if (!cpumask_test_cpu(cpu, tr->tracing_cpumask) &&
				cpumask_test_cpu(cpu, tracing_cpumask_new)) {
			atomic_dec(&per_cpu_ptr(tr->trace_buffer.data, cpu)->disabled);
			ring_buffer_record_enable_cpu(tr->trace_buffer.buffer, cpu);
		}
	}
	arch_spin_unlock(&tr->max_lock);
	local_irq_enable();

	cpumask_copy(tr->tracing_cpumask, tracing_cpumask_new);
	free_cpumask_var(tracing_cpumask_new);

	return count;

err_unlock:
	free_cpumask_var(tracing_cpumask_new);

	return err;
}