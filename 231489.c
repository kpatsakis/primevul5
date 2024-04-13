static ssize_t tracing_resize_ring_buffer(struct trace_array *tr,
					  unsigned long size, int cpu_id)
{
	int ret = size;

	mutex_lock(&trace_types_lock);

	if (cpu_id != RING_BUFFER_ALL_CPUS) {
		/* make sure, this cpu is enabled in the mask */
		if (!cpumask_test_cpu(cpu_id, tracing_buffer_mask)) {
			ret = -EINVAL;
			goto out;
		}
	}

	ret = __tracing_resize_ring_buffer(tr, size, cpu_id);
	if (ret < 0)
		ret = -ENOMEM;

out:
	mutex_unlock(&trace_types_lock);

	return ret;
}