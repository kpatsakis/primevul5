static void svm_pre_update_apicv_exec_ctrl(struct kvm *kvm, bool activate)
{
	avic_update_access_page(kvm, activate);
}