static int kvm_s390_get_tod(struct kvm *kvm, struct kvm_device_attr *attr)
{
	int ret;

	if (attr->flags)
		return -EINVAL;

	switch (attr->attr) {
	case KVM_S390_VM_TOD_EXT:
		ret = kvm_s390_get_tod_ext(kvm, attr);
		break;
	case KVM_S390_VM_TOD_HIGH:
		ret = kvm_s390_get_tod_high(kvm, attr);
		break;
	case KVM_S390_VM_TOD_LOW:
		ret = kvm_s390_get_tod_low(kvm, attr);
		break;
	default:
		ret = -ENXIO;
		break;
	}
	return ret;
}