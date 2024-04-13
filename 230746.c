static void set_tdp_cr3(struct kvm_vcpu *vcpu, unsigned long root)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	svm->vmcb->control.nested_cr3 = __sme_set(root);
	mark_dirty(svm->vmcb, VMCB_NPT);

	/* Also sync guest cr3 here in case we live migrate */
	svm->vmcb->save.cr3 = kvm_read_cr3(vcpu);
	mark_dirty(svm->vmcb, VMCB_CR);
}