static int kvm_hv_hypercall_complete(struct kvm_vcpu *vcpu, u64 result)
{
	kvm_hv_hypercall_set_result(vcpu, result);
	++vcpu->stat.hypercalls;
	return kvm_skip_emulated_instruction(vcpu);
}