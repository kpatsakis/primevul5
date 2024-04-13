static void kvm_hv_hypercall_set_result(struct kvm_vcpu *vcpu, u64 result)
{
	bool longmode;

	longmode = is_64_bit_mode(vcpu);
	if (longmode)
		kvm_rax_write(vcpu, result);
	else {
		kvm_rdx_write(vcpu, result >> 32);
		kvm_rax_write(vcpu, result & 0xffffffff);
	}
}