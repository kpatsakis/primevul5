static inline void trace_access_lock_init(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		mutex_init(&per_cpu(cpu_access_lock, cpu));
}