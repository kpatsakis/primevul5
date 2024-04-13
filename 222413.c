static void kvm_free_memslot(struct kvm *kvm, struct kvm_memory_slot *slot)
{
	kvm_destroy_dirty_bitmap(slot);

	kvm_arch_free_memslot(kvm, slot);

	slot->flags = 0;
	slot->npages = 0;
}