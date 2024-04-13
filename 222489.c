static inline int kvm_memslot_insert_back(struct kvm_memslots *slots)
{
	return slots->used_slots++;
}