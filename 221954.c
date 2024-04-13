static bool kvm_hv_is_syndbg_enabled(struct kvm_vcpu *vcpu)
{
	struct kvm_cpuid_entry2 *entry;

	entry = kvm_find_cpuid_entry(vcpu,
				     HYPERV_CPUID_SYNDBG_PLATFORM_CAPABILITIES,
				     0);
	if (!entry)
		return false;

	return entry->eax & HV_X64_SYNDBG_CAP_ALLOW_KERNEL_DEBUGGING;
}