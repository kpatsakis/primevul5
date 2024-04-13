static int kvm_s390_set_tod_high(struct kvm *kvm, struct kvm_device_attr *attr)
{
	u8 gtod_high;

	if (copy_from_user(&gtod_high, (void __user *)attr->addr,
					   sizeof(gtod_high)))
		return -EFAULT;

	if (gtod_high != 0)
		return -EINVAL;
	VM_EVENT(kvm, 3, "SET: TOD extension: 0x%x", gtod_high);

	return 0;
}