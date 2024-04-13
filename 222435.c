void kvm_get_kvm(struct kvm *kvm)
{
	refcount_inc(&kvm->users_count);
}