static void power_pmu_bhrb_reset(void)
{
	asm volatile(PPC_CLRBHRB);
}