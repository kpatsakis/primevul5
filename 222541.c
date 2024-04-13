int kvm_map_gfn(struct kvm_vcpu *vcpu, gfn_t gfn, struct kvm_host_map *map,
		struct gfn_to_pfn_cache *cache, bool atomic)
{
	return __kvm_map_gfn(kvm_memslots(vcpu->kvm), gfn, map,
			cache, atomic);
}