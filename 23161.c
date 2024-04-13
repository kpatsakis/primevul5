xt_get_this_cpu_counter(struct xt_counters *cnt)
{
	if (nr_cpu_ids > 1)
		return this_cpu_ptr((void __percpu *) (unsigned long) cnt->pcnt);

	return cnt;
}