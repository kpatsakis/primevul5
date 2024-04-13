static void nested_svm_set_tdp_cr3(struct kvm_vcpu *vcpu,
				   unsigned long root)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	svm->vmcb->control.nested_cr3 = root;
	mark_dirty(svm->vmcb, VMCB_NPT);
	svm_flush_tlb(vcpu);
}