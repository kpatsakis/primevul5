int kvm_vm_ioctl_check_extension(struct kvm *kvm, long ext)
{
	int r;

	switch (ext) {
	case KVM_CAP_S390_PSW:
	case KVM_CAP_S390_GMAP:
	case KVM_CAP_SYNC_MMU:
#ifdef CONFIG_KVM_S390_UCONTROL
	case KVM_CAP_S390_UCONTROL:
#endif
	case KVM_CAP_ASYNC_PF:
	case KVM_CAP_SYNC_REGS:
	case KVM_CAP_ONE_REG:
	case KVM_CAP_ENABLE_CAP:
	case KVM_CAP_S390_CSS_SUPPORT:
	case KVM_CAP_IOEVENTFD:
	case KVM_CAP_DEVICE_CTRL:
	case KVM_CAP_S390_IRQCHIP:
	case KVM_CAP_VM_ATTRIBUTES:
	case KVM_CAP_MP_STATE:
	case KVM_CAP_IMMEDIATE_EXIT:
	case KVM_CAP_S390_INJECT_IRQ:
	case KVM_CAP_S390_USER_SIGP:
	case KVM_CAP_S390_USER_STSI:
	case KVM_CAP_S390_SKEYS:
	case KVM_CAP_S390_IRQ_STATE:
	case KVM_CAP_S390_USER_INSTR0:
	case KVM_CAP_S390_CMMA_MIGRATION:
	case KVM_CAP_S390_AIS:
	case KVM_CAP_S390_AIS_MIGRATION:
	case KVM_CAP_S390_VCPU_RESETS:
		r = 1;
		break;
	case KVM_CAP_S390_HPAGE_1M:
		r = 0;
		if (hpage && !kvm_is_ucontrol(kvm))
			r = 1;
		break;
	case KVM_CAP_S390_MEM_OP:
		r = MEM_OP_MAX_SIZE;
		break;
	case KVM_CAP_NR_VCPUS:
	case KVM_CAP_MAX_VCPUS:
	case KVM_CAP_MAX_VCPU_ID:
		r = KVM_S390_BSCA_CPU_SLOTS;
		if (!kvm_s390_use_sca_entries())
			r = KVM_MAX_VCPUS;
		else if (sclp.has_esca && sclp.has_64bscao)
			r = KVM_S390_ESCA_CPU_SLOTS;
		break;
	case KVM_CAP_S390_COW:
		r = MACHINE_HAS_ESOP;
		break;
	case KVM_CAP_S390_VECTOR_REGISTERS:
		r = MACHINE_HAS_VX;
		break;
	case KVM_CAP_S390_RI:
		r = test_facility(64);
		break;
	case KVM_CAP_S390_GS:
		r = test_facility(133);
		break;
	case KVM_CAP_S390_BPB:
		r = test_facility(82);
		break;
	case KVM_CAP_S390_PROTECTED:
		r = is_prot_virt_host();
		break;
	default:
		r = 0;
	}
	return r;
}