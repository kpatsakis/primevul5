static inline struct kvm_io_bus *kvm_get_bus(struct kvm *kvm, enum kvm_bus idx)
{
	return srcu_dereference_check(kvm->buses[idx], &kvm->srcu,
				      lockdep_is_held(&kvm->slots_lock) ||
				      !refcount_read(&kvm->users_count));
}