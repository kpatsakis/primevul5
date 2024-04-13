static int kvm_s390_vm_stop_migration(struct kvm *kvm)
{
	/* migration mode already disabled */
	if (!kvm->arch.migration_mode)
		return 0;
	kvm->arch.migration_mode = 0;
	if (kvm->arch.use_cmma)
		kvm_s390_sync_request_broadcast(kvm, KVM_REQ_STOP_MIGRATION);
	return 0;
}