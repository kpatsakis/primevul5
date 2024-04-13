static int kvm_try_get_pfn(kvm_pfn_t pfn)
{
	if (kvm_is_reserved_pfn(pfn))
		return 1;
	return get_page_unless_zero(pfn_to_page(pfn));
}