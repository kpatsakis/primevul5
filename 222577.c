static int kvm_s390_get_tod_low(struct kvm *kvm, struct kvm_device_attr *attr)
{
	u64 gtod;

	gtod = kvm_s390_get_tod_clock_fast(kvm);
	if (copy_to_user((void __user *)attr->addr, &gtod, sizeof(gtod)))
		return -EFAULT;
	VM_EVENT(kvm, 3, "QUERY: TOD base: 0x%llx", gtod);

	return 0;
}