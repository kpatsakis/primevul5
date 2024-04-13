static void svm_set_dr6(struct kvm_vcpu *vcpu, unsigned long value)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	svm->vmcb->save.dr6 = value;
	mark_dirty(svm->vmcb, VMCB_DR);
}