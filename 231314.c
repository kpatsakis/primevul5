static inline void trace_access_lock(int cpu)
{
	if (cpu == RING_BUFFER_ALL_CPUS) {
		/* gain it for accessing the whole ring buffer. */
		down_write(&all_cpu_access_lock);
	} else {
		/* gain it for accessing a cpu ring buffer. */

		/* Firstly block other trace_access_lock(RING_BUFFER_ALL_CPUS). */
		down_read(&all_cpu_access_lock);

		/* Secondly block other access to this @cpu ring buffer. */
		mutex_lock(&per_cpu(cpu_access_lock, cpu));
	}
}