static void kvm_cache_gfn_to_pfn(struct kvm_memory_slot *slot, gfn_t gfn,
				 struct gfn_to_pfn_cache *cache, u64 gen)
{
	kvm_release_pfn(cache->pfn, cache->dirty, cache);

	cache->pfn = gfn_to_pfn_memslot(slot, gfn);
	cache->gfn = gfn;
	cache->dirty = false;
	cache->generation = gen;
}