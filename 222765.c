long kvm_arch_fault_in_page(struct kvm_vcpu *vcpu, gpa_t gpa, int writable)
{
	return gmap_fault(vcpu->arch.gmap, gpa,
			  writable ? FAULT_FLAG_WRITE : 0);
}