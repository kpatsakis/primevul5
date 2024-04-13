static u64 kvm_s390_get_initial_cpuid(void)
{
	struct cpuid cpuid;

	get_cpu_id(&cpuid);
	cpuid.version = 0xff;
	return *((u64 *) &cpuid);
}