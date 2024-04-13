static int kvm_s390_vm_get_migration(struct kvm *kvm,
				     struct kvm_device_attr *attr)
{
	u64 mig = kvm->arch.migration_mode;

	if (attr->attr != KVM_S390_VM_MIGRATION_STATUS)
		return -ENXIO;

	if (copy_to_user((void __user *)attr->addr, &mig, sizeof(mig)))
		return -EFAULT;
	return 0;
}