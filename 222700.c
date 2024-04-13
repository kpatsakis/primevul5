static int kvm_s390_get_tod_high(struct kvm *kvm, struct kvm_device_attr *attr)
{
	u8 gtod_high = 0;

	if (copy_to_user((void __user *)attr->addr, &gtod_high,
					 sizeof(gtod_high)))
		return -EFAULT;
	VM_EVENT(kvm, 3, "QUERY: TOD extension: 0x%x", gtod_high);

	return 0;
}