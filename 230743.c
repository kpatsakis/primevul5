static u64 svm_read_l1_tsc_offset(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	if (is_guest_mode(vcpu))
		return svm->nested.hsave->control.tsc_offset;

	return vcpu->arch.tsc_offset;
}