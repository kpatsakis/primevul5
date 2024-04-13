static inline void trace_access_lock(int cpu)
{
	(void)cpu;
	mutex_lock(&access_lock);
}