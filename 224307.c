static void nested_svm_intr(struct vcpu_svm *svm)
{
	trace_kvm_nested_intr_vmexit(svm->vmcb->save.rip);

	svm->vmcb->control.exit_code   = SVM_EXIT_INTR;
	svm->vmcb->control.exit_info_1 = 0;
	svm->vmcb->control.exit_info_2 = 0;

	nested_svm_vmexit(svm);
}