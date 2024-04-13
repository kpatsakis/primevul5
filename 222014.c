static int kvm_hv_set_msr(struct kvm_vcpu *vcpu, u32 msr, u64 data, bool host)
{
	struct kvm_vcpu_hv *hv_vcpu = to_hv_vcpu(vcpu);

	switch (msr) {
	case HV_X64_MSR_VP_INDEX: {
		struct kvm_hv *hv = to_kvm_hv(vcpu->kvm);
		int vcpu_idx = kvm_vcpu_get_idx(vcpu);
		u32 new_vp_index = (u32)data;

		if (!host || new_vp_index >= KVM_MAX_VCPUS)
			return 1;

		if (new_vp_index == hv_vcpu->vp_index)
			return 0;

		/*
		 * The VP index is initialized to vcpu_index by
		 * kvm_hv_vcpu_postcreate so they initially match.  Now the
		 * VP index is changing, adjust num_mismatched_vp_indexes if
		 * it now matches or no longer matches vcpu_idx.
		 */
		if (hv_vcpu->vp_index == vcpu_idx)
			atomic_inc(&hv->num_mismatched_vp_indexes);
		else if (new_vp_index == vcpu_idx)
			atomic_dec(&hv->num_mismatched_vp_indexes);

		hv_vcpu->vp_index = new_vp_index;
		break;
	}
	case HV_X64_MSR_VP_ASSIST_PAGE: {
		u64 gfn;
		unsigned long addr;

		if (!(data & HV_X64_MSR_VP_ASSIST_PAGE_ENABLE)) {
			hv_vcpu->hv_vapic = data;
			if (kvm_lapic_enable_pv_eoi(vcpu, 0, 0))
				return 1;
			break;
		}
		gfn = data >> HV_X64_MSR_VP_ASSIST_PAGE_ADDRESS_SHIFT;
		addr = kvm_vcpu_gfn_to_hva(vcpu, gfn);
		if (kvm_is_error_hva(addr))
			return 1;

		/*
		 * Clear apic_assist portion of struct hv_vp_assist_page
		 * only, there can be valuable data in the rest which needs
		 * to be preserved e.g. on migration.
		 */
		if (__put_user(0, (u32 __user *)addr))
			return 1;
		hv_vcpu->hv_vapic = data;
		kvm_vcpu_mark_page_dirty(vcpu, gfn);
		if (kvm_lapic_enable_pv_eoi(vcpu,
					    gfn_to_gpa(gfn) | KVM_MSR_ENABLED,
					    sizeof(struct hv_vp_assist_page)))
			return 1;
		break;
	}
	case HV_X64_MSR_EOI:
		return kvm_hv_vapic_msr_write(vcpu, APIC_EOI, data);
	case HV_X64_MSR_ICR:
		return kvm_hv_vapic_msr_write(vcpu, APIC_ICR, data);
	case HV_X64_MSR_TPR:
		return kvm_hv_vapic_msr_write(vcpu, APIC_TASKPRI, data);
	case HV_X64_MSR_VP_RUNTIME:
		if (!host)
			return 1;
		hv_vcpu->runtime_offset = data - current_task_runtime_100ns();
		break;
	case HV_X64_MSR_SCONTROL:
	case HV_X64_MSR_SVERSION:
	case HV_X64_MSR_SIEFP:
	case HV_X64_MSR_SIMP:
	case HV_X64_MSR_EOM:
	case HV_X64_MSR_SINT0 ... HV_X64_MSR_SINT15:
		return synic_set_msr(to_hv_synic(vcpu), msr, data, host);
	case HV_X64_MSR_STIMER0_CONFIG:
	case HV_X64_MSR_STIMER1_CONFIG:
	case HV_X64_MSR_STIMER2_CONFIG:
	case HV_X64_MSR_STIMER3_CONFIG: {
		int timer_index = (msr - HV_X64_MSR_STIMER0_CONFIG)/2;

		return stimer_set_config(to_hv_stimer(vcpu, timer_index),
					 data, host);
	}
	case HV_X64_MSR_STIMER0_COUNT:
	case HV_X64_MSR_STIMER1_COUNT:
	case HV_X64_MSR_STIMER2_COUNT:
	case HV_X64_MSR_STIMER3_COUNT: {
		int timer_index = (msr - HV_X64_MSR_STIMER0_COUNT)/2;

		return stimer_set_count(to_hv_stimer(vcpu, timer_index),
					data, host);
	}
	case HV_X64_MSR_TSC_FREQUENCY:
	case HV_X64_MSR_APIC_FREQUENCY:
		/* read-only, but still ignore it if host-initiated */
		if (!host)
			return 1;
		break;
	default:
		vcpu_unimpl(vcpu, "Hyper-V unhandled wrmsr: 0x%x data 0x%llx\n",
			    msr, data);
		return 1;
	}

	return 0;
}