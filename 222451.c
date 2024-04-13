static inline bool kvm_vcpu_mapped(struct kvm_host_map *map)
{
	return !!map->hva;
}