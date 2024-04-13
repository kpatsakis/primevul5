xt_get_per_cpu_counter(struct xt_counters *cnt, unsigned int cpu)
{
	if (nr_cpu_ids > 1)
		return per_cpu_ptr((void __percpu *) (unsigned long) cnt->pcnt, cpu);

	return cnt;
}