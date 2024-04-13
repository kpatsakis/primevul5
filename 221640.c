static void __init mds_print_mitigation(void)
{
	if (!boot_cpu_has_bug(X86_BUG_MDS) || cpu_mitigations_off())
		return;

	pr_info("%s\n", mds_strings[mds_mitigation]);
}