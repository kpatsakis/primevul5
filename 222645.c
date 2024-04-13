static int __kvm_map_gfn(struct kvm_memslots *slots, gfn_t gfn,
			 struct kvm_host_map *map,
			 struct gfn_to_pfn_cache *cache,
			 bool atomic)
{
	kvm_pfn_t pfn;
	void *hva = NULL;
	struct page *page = KVM_UNMAPPED_PAGE;
	struct kvm_memory_slot *slot = __gfn_to_memslot(slots, gfn);
	u64 gen = slots->generation;

	if (!map)
		return -EINVAL;

	if (cache) {
		if (!cache->pfn || cache->gfn != gfn ||
			cache->generation != gen) {
			if (atomic)
				return -EAGAIN;
			kvm_cache_gfn_to_pfn(slot, gfn, cache, gen);
		}
		pfn = cache->pfn;
	} else {
		if (atomic)
			return -EAGAIN;
		pfn = gfn_to_pfn_memslot(slot, gfn);
	}
	if (is_error_noslot_pfn(pfn))
		return -EINVAL;

	if (pfn_valid(pfn)) {
		page = pfn_to_page(pfn);
		if (atomic)
			hva = kmap_atomic(page);
		else
			hva = kmap(page);
#ifdef CONFIG_HAS_IOMEM
	} else if (!atomic) {
		hva = memremap(pfn_to_hpa(pfn), PAGE_SIZE, MEMREMAP_WB);
	} else {
		return -EINVAL;
#endif
	}

	if (!hva)
		return -EFAULT;

	map->page = page;
	map->hva = hva;
	map->pfn = pfn;
	map->gfn = gfn;

	return 0;
}