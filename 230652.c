static void svm_vm_free(struct kvm *kvm)
{
	vfree(to_kvm_svm(kvm));
}