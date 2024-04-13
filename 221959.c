bool kvm_hv_hypercall_enabled(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.hyperv_enabled && to_kvm_hv(vcpu->kvm)->hv_guest_os_id;
}