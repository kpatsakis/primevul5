static inline void trace_access_unlock(int cpu)
{
	if (cpu == RING_BUFFER_ALL_CPUS) {
		up_write(&all_cpu_access_lock);
	} else {
		mutex_unlock(&per_cpu(cpu_access_lock, cpu));
		up_read(&all_cpu_access_lock);
	}
}