static int avic_incomplete_ipi_interception(struct vcpu_svm *svm)
{
	u32 icrh = svm->vmcb->control.exit_info_1 >> 32;
	u32 icrl = svm->vmcb->control.exit_info_1;
	u32 id = svm->vmcb->control.exit_info_2 >> 32;
	u32 index = svm->vmcb->control.exit_info_2 & 0xFF;
	struct kvm_lapic *apic = svm->vcpu.arch.apic;

	trace_kvm_avic_incomplete_ipi(svm->vcpu.vcpu_id, icrh, icrl, id, index);

	switch (id) {
	case AVIC_IPI_FAILURE_INVALID_INT_TYPE:
		/*
		 * AVIC hardware handles the generation of
		 * IPIs when the specified Message Type is Fixed
		 * (also known as fixed delivery mode) and
		 * the Trigger Mode is edge-triggered. The hardware
		 * also supports self and broadcast delivery modes
		 * specified via the Destination Shorthand(DSH)
		 * field of the ICRL. Logical and physical APIC ID
		 * formats are supported. All other IPI types cause
		 * a #VMEXIT, which needs to emulated.
		 */
		kvm_lapic_reg_write(apic, APIC_ICR2, icrh);
		kvm_lapic_reg_write(apic, APIC_ICR, icrl);
		break;
	case AVIC_IPI_FAILURE_TARGET_NOT_RUNNING: {
		int i;
		struct kvm_vcpu *vcpu;
		struct kvm *kvm = svm->vcpu.kvm;
		struct kvm_lapic *apic = svm->vcpu.arch.apic;

		/*
		 * At this point, we expect that the AVIC HW has already
		 * set the appropriate IRR bits on the valid target
		 * vcpus. So, we just need to kick the appropriate vcpu.
		 */
		kvm_for_each_vcpu(i, vcpu, kvm) {
			bool m = kvm_apic_match_dest(vcpu, apic,
						     icrl & APIC_SHORT_MASK,
						     GET_APIC_DEST_FIELD(icrh),
						     icrl & APIC_DEST_MASK);

			if (m && !avic_vcpu_is_running(vcpu))
				kvm_vcpu_wake_up(vcpu);
		}
		break;
	}
	case AVIC_IPI_FAILURE_INVALID_TARGET:
		WARN_ONCE(1, "Invalid IPI target: index=%u, vcpu=%d, icr=%#0x:%#0x\n",
			  index, svm->vcpu.vcpu_id, icrh, icrl);
		break;
	case AVIC_IPI_FAILURE_INVALID_BACKING_PAGE:
		WARN_ONCE(1, "Invalid backing page\n");
		break;
	default:
		pr_err("Unknown IPI interception\n");
	}

	return 1;
}