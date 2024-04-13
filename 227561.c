static void __kvm_unmap_gfn(struct kvm *kvm,
			struct kvm_memory_slot *memslot,
			struct kvm_host_map *map,
			struct gfn_to_pfn_cache *cache,
			bool dirty, bool atomic)
{
	if (!map)
		return;

	if (!map->hva)
		return;

	if (map->page != KVM_UNMAPPED_PAGE) {
		if (atomic)
			kunmap_atomic(map->hva);
		else
			kunmap(map->page);
	}
#ifdef CONFIG_HAS_IOMEM
	else if (!atomic)
		memunmap(map->hva);
	else
		WARN_ONCE(1, "Unexpected unmapping in atomic context");
#endif

	if (dirty)
		mark_page_dirty_in_slot(kvm, memslot, map->gfn);

	if (cache)
		cache->dirty |= dirty;
	else
		kvm_release_pfn(map->pfn, dirty, NULL);

	map->hva = NULL;
	map->page = NULL;
}