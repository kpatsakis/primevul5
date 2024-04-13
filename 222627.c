static int kvm_s390_set_cpu_model(struct kvm *kvm, struct kvm_device_attr *attr)
{
	int ret = -ENXIO;

	switch (attr->attr) {
	case KVM_S390_VM_CPU_PROCESSOR:
		ret = kvm_s390_set_processor(kvm, attr);
		break;
	case KVM_S390_VM_CPU_PROCESSOR_FEAT:
		ret = kvm_s390_set_processor_feat(kvm, attr);
		break;
	case KVM_S390_VM_CPU_PROCESSOR_SUBFUNC:
		ret = kvm_s390_set_processor_subfunc(kvm, attr);
		break;
	}
	return ret;
}