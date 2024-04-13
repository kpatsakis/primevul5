bool kvm_is_transparent_hugepage(kvm_pfn_t pfn)
{
	struct page *page = pfn_to_page(pfn);

	if (!PageTransCompoundMap(page))
		return false;

	return is_transparent_hugepage(compound_head(page));
}