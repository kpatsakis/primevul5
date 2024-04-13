static bool svm_xsaves_supported(void)
{
	return boot_cpu_has(X86_FEATURE_XSAVES);
}