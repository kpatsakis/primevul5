int register_power_pmu(struct power_pmu *pmu)
{
	if (ppmu)
		return -EBUSY;		/* something's already registered */

	ppmu = pmu;
	pr_info("%s performance monitor hardware support registered\n",
		pmu->name);

	power_pmu.attr_groups = ppmu->attr_groups;
	power_pmu.capabilities |= (ppmu->capabilities & PERF_PMU_CAP_EXTENDED_REGS);

#ifdef MSR_HV
	/*
	 * Use FCHV to ignore kernel events if MSR.HV is set.
	 */
	if (mfmsr() & MSR_HV)
		freeze_events_kernel = MMCR0_FCHV;
#endif /* CONFIG_PPC64 */

	perf_pmu_register(&power_pmu, "cpu", PERF_TYPE_RAW);
	cpuhp_setup_state(CPUHP_PERF_POWER, "perf/powerpc:prepare",
			  power_pmu_prepare_cpu, NULL);
	return 0;
}