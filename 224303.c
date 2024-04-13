int svm_allocate_nested(struct vcpu_svm *svm)
{
	struct page *hsave_page;

	if (svm->nested.initialized)
		return 0;

	hsave_page = alloc_page(GFP_KERNEL_ACCOUNT | __GFP_ZERO);
	if (!hsave_page)
		return -ENOMEM;
	svm->nested.hsave = page_address(hsave_page);

	svm->nested.msrpm = svm_vcpu_alloc_msrpm();
	if (!svm->nested.msrpm)
		goto err_free_hsave;
	svm_vcpu_init_msrpm(&svm->vcpu, svm->nested.msrpm);

	svm->nested.initialized = true;
	return 0;

err_free_hsave:
	__free_page(hsave_page);
	return -ENOMEM;
}