static int __init setup_numabalancing(char *str)
{
	int ret = 0;
	if (!str)
		goto out;

	if (!strcmp(str, "enable")) {
		numabalancing_override = 1;
		ret = 1;
	} else if (!strcmp(str, "disable")) {
		numabalancing_override = -1;
		ret = 1;
	}
out:
	if (!ret)
		pr_warn("Unable to parse numa_balancing=\n");

	return ret;
}