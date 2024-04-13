static void nested_svm_init(struct vcpu_svm *svm)
{
	svm->vmcb->control.exit_code   = SVM_EXIT_INIT;
	svm->vmcb->control.exit_info_1 = 0;
	svm->vmcb->control.exit_info_2 = 0;

	nested_svm_vmexit(svm);
}