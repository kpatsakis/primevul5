static inline struct kvm_memslots *__kvm_memslots(struct kvm *kvm, int as_id)
{
	as_id = array_index_nospec(as_id, KVM_ADDRESS_SPACE_NUM);
	return srcu_dereference_check(kvm->memslots[as_id], &kvm->srcu,
			lockdep_is_held(&kvm->slots_lock) ||
			!refcount_read(&kvm->users_count));
}