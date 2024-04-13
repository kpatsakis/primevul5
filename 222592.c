static int kvm_vm_ioctl_clear_dirty_log(struct kvm *kvm,
					struct kvm_clear_dirty_log *log)
{
	int r;

	mutex_lock(&kvm->slots_lock);

	r = kvm_clear_dirty_log_protect(kvm, log);

	mutex_unlock(&kvm->slots_lock);
	return r;
}