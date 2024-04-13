static int kvm_hv_hypercall_complete_userspace(struct kvm_vcpu *vcpu)
{
	return kvm_hv_hypercall_complete(vcpu, vcpu->run->hyperv.u.hcall.result);
}