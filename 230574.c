static int avic_unaccel_trap_write(struct vcpu_svm *svm)
{
	struct kvm_lapic *apic = svm->vcpu.arch.apic;
	u32 offset = svm->vmcb->control.exit_info_1 &
				AVIC_UNACCEL_ACCESS_OFFSET_MASK;

	switch (offset) {
	case APIC_ID:
		if (avic_handle_apic_id_update(&svm->vcpu))
			return 0;
		break;
	case APIC_LDR:
		if (avic_handle_ldr_update(&svm->vcpu))
			return 0;
		break;
	case APIC_DFR:
		avic_handle_dfr_update(&svm->vcpu);
		break;
	default:
		break;
	}

	kvm_lapic_reg_write(apic, offset, kvm_lapic_get_reg(apic, offset));

	return 1;
}