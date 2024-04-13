void svm_free_nested(struct vcpu_svm *svm)
{
	if (!svm->nested.initialized)
		return;

	svm_vcpu_free_msrpm(svm->nested.msrpm);
	svm->nested.msrpm = NULL;

	__free_page(virt_to_page(svm->nested.hsave));
	svm->nested.hsave = NULL;

	svm->nested.initialized = false;
}