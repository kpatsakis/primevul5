static bool svm_rdtscp_supported(void)
{
	return boot_cpu_has(X86_FEATURE_RDTSCP);
}