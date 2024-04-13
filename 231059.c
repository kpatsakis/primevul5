static u64 svm_read_tsc_offset(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	return svm->vmcb->control.tsc_offset;
}