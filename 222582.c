static int kvm_s390_get_tod_ext(struct kvm *kvm, struct kvm_device_attr *attr)
{
	struct kvm_s390_vm_tod_clock gtod;

	memset(&gtod, 0, sizeof(gtod));
	kvm_s390_get_tod_clock(kvm, &gtod);
	if (copy_to_user((void __user *)attr->addr, &gtod, sizeof(gtod)))
		return -EFAULT;

	VM_EVENT(kvm, 3, "QUERY: TOD extension: 0x%x, TOD base: 0x%llx",
		gtod.epoch_idx, gtod.tod);
	return 0;
}