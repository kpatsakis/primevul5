static inline bool kvm_dirty_log_manual_protect_and_init_set(struct kvm *kvm)
{
	return !!(kvm->manual_dirty_log_protect & KVM_DIRTY_LOG_INITIALLY_SET);
}