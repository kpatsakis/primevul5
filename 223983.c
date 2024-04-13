static int __init init_ppc64_pmu(void)
{
	/* run through all the pmu drivers one at a time */
	if (!init_power5_pmu())
		return 0;
	else if (!init_power5p_pmu())
		return 0;
	else if (!init_power6_pmu())
		return 0;
	else if (!init_power7_pmu())
		return 0;
	else if (!init_power8_pmu())
		return 0;
	else if (!init_power9_pmu())
		return 0;
	else if (!init_power10_pmu())
		return 0;
	else if (!init_ppc970_pmu())
		return 0;
	else
		return init_generic_compat_pmu();
}