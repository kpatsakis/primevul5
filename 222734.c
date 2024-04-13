static int kvm_s390_set_tod_ext(struct kvm *kvm, struct kvm_device_attr *attr)
{
	struct kvm_s390_vm_tod_clock gtod;

	if (copy_from_user(&gtod, (void __user *)attr->addr, sizeof(gtod)))
		return -EFAULT;

	if (!test_kvm_facility(kvm, 139) && gtod.epoch_idx)
		return -EINVAL;
	kvm_s390_set_tod_clock(kvm, &gtod);

	VM_EVENT(kvm, 3, "SET: TOD extension: 0x%x, TOD base: 0x%llx",
		gtod.epoch_idx, gtod.tod);

	return 0;
}