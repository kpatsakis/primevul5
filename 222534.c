static int kvm_s390_set_tod_low(struct kvm *kvm, struct kvm_device_attr *attr)
{
	struct kvm_s390_vm_tod_clock gtod = { 0 };

	if (copy_from_user(&gtod.tod, (void __user *)attr->addr,
			   sizeof(gtod.tod)))
		return -EFAULT;

	kvm_s390_set_tod_clock(kvm, &gtod);
	VM_EVENT(kvm, 3, "SET: TOD base: 0x%llx", gtod.tod);
	return 0;
}