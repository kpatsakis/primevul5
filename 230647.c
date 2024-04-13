static u64 svm_get_dr6(struct kvm_vcpu *vcpu)
{
	return to_svm(vcpu)->vmcb->save.dr6;
}