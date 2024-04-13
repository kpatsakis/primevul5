static int kvm_s390_vm_set_migration(struct kvm *kvm,
				     struct kvm_device_attr *attr)
{
	int res = -ENXIO;

	mutex_lock(&kvm->slots_lock);
	switch (attr->attr) {
	case KVM_S390_VM_MIGRATION_START:
		res = kvm_s390_vm_start_migration(kvm);
		break;
	case KVM_S390_VM_MIGRATION_STOP:
		res = kvm_s390_vm_stop_migration(kvm);
		break;
	default:
		break;
	}
	mutex_unlock(&kvm->slots_lock);

	return res;
}