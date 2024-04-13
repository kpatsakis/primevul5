static int kvm_s390_vm_get_attr(struct kvm *kvm, struct kvm_device_attr *attr)
{
	int ret;

	switch (attr->group) {
	case KVM_S390_VM_MEM_CTRL:
		ret = kvm_s390_get_mem_control(kvm, attr);
		break;
	case KVM_S390_VM_TOD:
		ret = kvm_s390_get_tod(kvm, attr);
		break;
	case KVM_S390_VM_CPU_MODEL:
		ret = kvm_s390_get_cpu_model(kvm, attr);
		break;
	case KVM_S390_VM_MIGRATION:
		ret = kvm_s390_vm_get_migration(kvm, attr);
		break;
	default:
		ret = -ENXIO;
		break;
	}

	return ret;
}