static int is_limited_pmc(int pmcnum)
{
	return (ppmu->flags & PPMU_LIMITED_PMC5_6)
		&& (pmcnum == 5 || pmcnum == 6);
}