void kvm_release_pfn(kvm_pfn_t pfn, bool dirty, struct gfn_to_pfn_cache *cache)
{
	if (pfn == 0)
		return;

	if (cache)
		cache->pfn = cache->gfn = 0;

	if (dirty)
		kvm_release_pfn_dirty(pfn);
	else
		kvm_release_pfn_clean(pfn);
}