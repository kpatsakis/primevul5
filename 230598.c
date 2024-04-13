static void sev_unpin_memory(struct kvm *kvm, struct page **pages,
			     unsigned long npages)
{
	struct kvm_sev_info *sev = &to_kvm_svm(kvm)->sev_info;

	release_pages(pages, npages);
	kvfree(pages);
	sev->pages_locked -= npages;
}