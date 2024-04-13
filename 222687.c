static inline unsigned long *kvm_second_dirty_bitmap(struct kvm_memory_slot *memslot)
{
	unsigned long len = kvm_dirty_bitmap_bytes(memslot);

	return memslot->dirty_bitmap + len / sizeof(*memslot->dirty_bitmap);
}