static int kvm_s390_get_mem_control(struct kvm *kvm, struct kvm_device_attr *attr)
{
	int ret;

	switch (attr->attr) {
	case KVM_S390_VM_MEM_LIMIT_SIZE:
		ret = 0;
		VM_EVENT(kvm, 3, "QUERY: max guest memory: %lu bytes",
			 kvm->arch.mem_limit);
		if (put_user(kvm->arch.mem_limit, (u64 __user *)attr->addr))
			ret = -EFAULT;
		break;
	default:
		ret = -ENXIO;
		break;
	}
	return ret;
}