static int kvm_s390_get_cpu_model(struct kvm *kvm, struct kvm_device_attr *attr)
{
	int ret = -ENXIO;

	switch (attr->attr) {
	case KVM_S390_VM_CPU_PROCESSOR:
		ret = kvm_s390_get_processor(kvm, attr);
		break;
	case KVM_S390_VM_CPU_MACHINE:
		ret = kvm_s390_get_machine(kvm, attr);
		break;
	case KVM_S390_VM_CPU_PROCESSOR_FEAT:
		ret = kvm_s390_get_processor_feat(kvm, attr);
		break;
	case KVM_S390_VM_CPU_MACHINE_FEAT:
		ret = kvm_s390_get_machine_feat(kvm, attr);
		break;
	case KVM_S390_VM_CPU_PROCESSOR_SUBFUNC:
		ret = kvm_s390_get_processor_subfunc(kvm, attr);
		break;
	case KVM_S390_VM_CPU_MACHINE_SUBFUNC:
		ret = kvm_s390_get_machine_subfunc(kvm, attr);
		break;
	}
	return ret;
}