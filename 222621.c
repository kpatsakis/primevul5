int kvm_arch_vcpu_ioctl_set_guest_debug(struct kvm_vcpu *vcpu,
					struct kvm_guest_debug *dbg)
{
	int rc = 0;

	vcpu_load(vcpu);

	vcpu->guest_debug = 0;
	kvm_s390_clear_bp_data(vcpu);

	if (dbg->control & ~VALID_GUESTDBG_FLAGS) {
		rc = -EINVAL;
		goto out;
	}
	if (!sclp.has_gpere) {
		rc = -EINVAL;
		goto out;
	}

	if (dbg->control & KVM_GUESTDBG_ENABLE) {
		vcpu->guest_debug = dbg->control;
		/* enforce guest PER */
		kvm_s390_set_cpuflags(vcpu, CPUSTAT_P);

		if (dbg->control & KVM_GUESTDBG_USE_HW_BP)
			rc = kvm_s390_import_bp_data(vcpu, dbg);
	} else {
		kvm_s390_clear_cpuflags(vcpu, CPUSTAT_P);
		vcpu->arch.guestdbg.last_bp = 0;
	}

	if (rc) {
		vcpu->guest_debug = 0;
		kvm_s390_clear_bp_data(vcpu);
		kvm_s390_clear_cpuflags(vcpu, CPUSTAT_P);
	}

out:
	vcpu_put(vcpu);
	return rc;
}