__gfn_to_hva_memslot(struct kvm_memory_slot *slot, gfn_t gfn)
{
	return slot->userspace_addr + (gfn - slot->base_gfn) * PAGE_SIZE;
}