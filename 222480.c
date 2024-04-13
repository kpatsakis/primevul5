static int kvm_s390_vm_has_attr(struct kvm *kvm, struct kvm_device_attr *attr)
{
	int ret;

	switch (attr->group) {
	case KVM_S390_VM_MEM_CTRL:
		switch (attr->attr) {
		case KVM_S390_VM_MEM_ENABLE_CMMA:
		case KVM_S390_VM_MEM_CLR_CMMA:
			ret = sclp.has_cmma ? 0 : -ENXIO;
			break;
		case KVM_S390_VM_MEM_LIMIT_SIZE:
			ret = 0;
			break;
		default:
			ret = -ENXIO;
			break;
		}
		break;
	case KVM_S390_VM_TOD:
		switch (attr->attr) {
		case KVM_S390_VM_TOD_LOW:
		case KVM_S390_VM_TOD_HIGH:
			ret = 0;
			break;
		default:
			ret = -ENXIO;
			break;
		}
		break;
	case KVM_S390_VM_CPU_MODEL:
		switch (attr->attr) {
		case KVM_S390_VM_CPU_PROCESSOR:
		case KVM_S390_VM_CPU_MACHINE:
		case KVM_S390_VM_CPU_PROCESSOR_FEAT:
		case KVM_S390_VM_CPU_MACHINE_FEAT:
		case KVM_S390_VM_CPU_MACHINE_SUBFUNC:
		case KVM_S390_VM_CPU_PROCESSOR_SUBFUNC:
			ret = 0;
			break;
		default:
			ret = -ENXIO;
			break;
		}
		break;
	case KVM_S390_VM_CRYPTO:
		switch (attr->attr) {
		case KVM_S390_VM_CRYPTO_ENABLE_AES_KW:
		case KVM_S390_VM_CRYPTO_ENABLE_DEA_KW:
		case KVM_S390_VM_CRYPTO_DISABLE_AES_KW:
		case KVM_S390_VM_CRYPTO_DISABLE_DEA_KW:
			ret = 0;
			break;
		case KVM_S390_VM_CRYPTO_ENABLE_APIE:
		case KVM_S390_VM_CRYPTO_DISABLE_APIE:
			ret = ap_instructions_available() ? 0 : -ENXIO;
			break;
		default:
			ret = -ENXIO;
			break;
		}
		break;
	case KVM_S390_VM_MIGRATION:
		ret = 0;
		break;
	default:
		ret = -ENXIO;
		break;
	}

	return ret;
}