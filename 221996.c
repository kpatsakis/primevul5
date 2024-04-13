void kvm_hv_set_cpuid(struct kvm_vcpu *vcpu)
{
	struct kvm_cpuid_entry2 *entry;

	entry = kvm_find_cpuid_entry(vcpu, HYPERV_CPUID_INTERFACE, 0);
	if (entry && entry->eax == HYPERV_CPUID_SIGNATURE_EAX)
		vcpu->arch.hyperv_enabled = true;
	else
		vcpu->arch.hyperv_enabled = false;
}