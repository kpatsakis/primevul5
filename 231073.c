static void *nested_svm_map(struct vcpu_svm *svm, u64 gpa, struct page **_page)
{
	struct page *page;

	might_sleep();

	page = gfn_to_page(svm->vcpu.kvm, gpa >> PAGE_SHIFT);
	if (is_error_page(page))
		goto error;

	*_page = page;

	return kmap(page);

error:
	kvm_inject_gp(&svm->vcpu, 0);

	return NULL;
}