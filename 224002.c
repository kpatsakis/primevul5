static void power_pmu_start_txn(struct pmu *pmu, unsigned int txn_flags)
{
	struct cpu_hw_events *cpuhw = this_cpu_ptr(&cpu_hw_events);

	WARN_ON_ONCE(cpuhw->txn_flags);		/* txn already in flight */

	cpuhw->txn_flags = txn_flags;
	if (txn_flags & ~PERF_PMU_TXN_ADD)
		return;

	perf_pmu_disable(pmu);
	cpuhw->n_txn_start = cpuhw->n_events;
}