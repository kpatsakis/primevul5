static struct kvm *svm_vm_alloc(void)
{
	struct kvm_svm *kvm_svm = __vmalloc(sizeof(struct kvm_svm),
					    GFP_KERNEL_ACCOUNT | __GFP_ZERO,
					    PAGE_KERNEL);
	return &kvm_svm->kvm;
}