static void power_pmu_cancel_txn(struct pmu *pmu)
{
	struct cpu_hw_events *cpuhw = this_cpu_ptr(&cpu_hw_events);
	unsigned int txn_flags;

	WARN_ON_ONCE(!cpuhw->txn_flags);	/* no txn in flight */

	txn_flags = cpuhw->txn_flags;
	cpuhw->txn_flags = 0;
	if (txn_flags & ~PERF_PMU_TXN_ADD)
		return;

	perf_pmu_enable(pmu);
}