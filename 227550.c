int kvm_unmap_gfn(struct kvm_vcpu *vcpu, struct kvm_host_map *map, 
		  struct gfn_to_pfn_cache *cache, bool dirty, bool atomic)
{
	__kvm_unmap_gfn(vcpu->kvm, gfn_to_memslot(vcpu->kvm, map->gfn), map,
			cache, dirty, atomic);
	return 0;
}