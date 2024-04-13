static int kvm_hv_set_msr_pw(struct kvm_vcpu *vcpu, u32 msr, u64 data,
			     bool host)
{
	struct kvm *kvm = vcpu->kvm;
	struct kvm_hv *hv = to_kvm_hv(kvm);

	switch (msr) {
	case HV_X64_MSR_GUEST_OS_ID:
		hv->hv_guest_os_id = data;
		/* setting guest os id to zero disables hypercall page */
		if (!hv->hv_guest_os_id)
			hv->hv_hypercall &= ~HV_X64_MSR_HYPERCALL_ENABLE;
		break;
	case HV_X64_MSR_HYPERCALL: {
		u8 instructions[9];
		int i = 0;
		u64 addr;

		/* if guest os id is not set hypercall should remain disabled */
		if (!hv->hv_guest_os_id)
			break;
		if (!(data & HV_X64_MSR_HYPERCALL_ENABLE)) {
			hv->hv_hypercall = data;
			break;
		}

		/*
		 * If Xen and Hyper-V hypercalls are both enabled, disambiguate
		 * the same way Xen itself does, by setting the bit 31 of EAX
		 * which is RsvdZ in the 32-bit Hyper-V hypercall ABI and just
		 * going to be clobbered on 64-bit.
		 */
		if (kvm_xen_hypercall_enabled(kvm)) {
			/* orl $0x80000000, %eax */
			instructions[i++] = 0x0d;
			instructions[i++] = 0x00;
			instructions[i++] = 0x00;
			instructions[i++] = 0x00;
			instructions[i++] = 0x80;
		}

		/* vmcall/vmmcall */
		static_call(kvm_x86_patch_hypercall)(vcpu, instructions + i);
		i += 3;

		/* ret */
		((unsigned char *)instructions)[i++] = 0xc3;

		addr = data & HV_X64_MSR_HYPERCALL_PAGE_ADDRESS_MASK;
		if (kvm_vcpu_write_guest(vcpu, addr, instructions, i))
			return 1;
		hv->hv_hypercall = data;
		break;
	}
	case HV_X64_MSR_REFERENCE_TSC:
		hv->hv_tsc_page = data;
		if (hv->hv_tsc_page & HV_X64_MSR_TSC_REFERENCE_ENABLE)
			kvm_make_request(KVM_REQ_MASTERCLOCK_UPDATE, vcpu);
		break;
	case HV_X64_MSR_CRASH_P0 ... HV_X64_MSR_CRASH_P4:
		return kvm_hv_msr_set_crash_data(kvm,
						 msr - HV_X64_MSR_CRASH_P0,
						 data);
	case HV_X64_MSR_CRASH_CTL:
		if (host)
			return kvm_hv_msr_set_crash_ctl(kvm, data);

		if (data & HV_CRASH_CTL_CRASH_NOTIFY) {
			vcpu_debug(vcpu, "hv crash (0x%llx 0x%llx 0x%llx 0x%llx 0x%llx)\n",
				   hv->hv_crash_param[0],
				   hv->hv_crash_param[1],
				   hv->hv_crash_param[2],
				   hv->hv_crash_param[3],
				   hv->hv_crash_param[4]);

			/* Send notification about crash to user space */
			kvm_make_request(KVM_REQ_HV_CRASH, vcpu);
		}
		break;
	case HV_X64_MSR_RESET:
		if (data == 1) {
			vcpu_debug(vcpu, "hyper-v reset requested\n");
			kvm_make_request(KVM_REQ_HV_RESET, vcpu);
		}
		break;
	case HV_X64_MSR_REENLIGHTENMENT_CONTROL:
		hv->hv_reenlightenment_control = data;
		break;
	case HV_X64_MSR_TSC_EMULATION_CONTROL:
		hv->hv_tsc_emulation_control = data;
		break;
	case HV_X64_MSR_TSC_EMULATION_STATUS:
		hv->hv_tsc_emulation_status = data;
		break;
	case HV_X64_MSR_TIME_REF_COUNT:
		/* read-only, but still ignore it if host-initiated */
		if (!host)
			return 1;
		break;
	case HV_X64_MSR_SYNDBG_OPTIONS:
	case HV_X64_MSR_SYNDBG_CONTROL ... HV_X64_MSR_SYNDBG_PENDING_BUFFER:
		return syndbg_set_msr(vcpu, msr, data, host);
	default:
		vcpu_unimpl(vcpu, "Hyper-V unhandled wrmsr: 0x%x data 0x%llx\n",
			    msr, data);
		return 1;
	}
	return 0;
}