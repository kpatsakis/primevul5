int kvm_read_guest_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
			  void *data, unsigned long len)
{
	return kvm_read_guest_offset_cached(kvm, ghc, data, 0, len);
}