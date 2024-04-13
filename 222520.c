struct kvm_memory_slot *id_to_memslot(struct kvm_memslots *slots, int id)
{
	int index = slots->id_to_index[id];
	struct kvm_memory_slot *slot;

	if (index < 0)
		return NULL;

	slot = &slots->memslots[index];

	WARN_ON(slot->id != id);
	return slot;
}