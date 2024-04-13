static void __init check_numabalancing_enable(void)
{
	bool numabalancing_default = false;

	if (IS_ENABLED(CONFIG_NUMA_BALANCING_DEFAULT_ENABLED))
		numabalancing_default = true;

	/* Parsed by setup_numabalancing. override == 1 enables, -1 disables */
	if (numabalancing_override)
		set_numabalancing_state(numabalancing_override == 1);

	if (num_online_nodes() > 1 && !numabalancing_override) {
		pr_info("%s automatic NUMA balancing. Configure with numa_balancing= or the kernel.numa_balancing sysctl\n",
			numabalancing_default ? "Enabling" : "Disabling");
		set_numabalancing_state(numabalancing_default);
	}
}