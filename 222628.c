int kvm_arch_vcpu_ioctl_run(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	int rc;

	if (kvm_run->immediate_exit)
		return -EINTR;

	if (kvm_run->kvm_valid_regs & ~KVM_SYNC_S390_VALID_FIELDS ||
	    kvm_run->kvm_dirty_regs & ~KVM_SYNC_S390_VALID_FIELDS)
		return -EINVAL;

	vcpu_load(vcpu);

	if (guestdbg_exit_pending(vcpu)) {
		kvm_s390_prepare_debug_exit(vcpu);
		rc = 0;
		goto out;
	}

	kvm_sigset_activate(vcpu);

	/*
	 * no need to check the return value of vcpu_start as it can only have
	 * an error for protvirt, but protvirt means user cpu state
	 */
	if (!kvm_s390_user_cpu_state_ctrl(vcpu->kvm)) {
		kvm_s390_vcpu_start(vcpu);
	} else if (is_vcpu_stopped(vcpu)) {
		pr_err_ratelimited("can't run stopped vcpu %d\n",
				   vcpu->vcpu_id);
		rc = -EINVAL;
		goto out;
	}

	sync_regs(vcpu, kvm_run);
	enable_cpu_timer_accounting(vcpu);

	might_fault();
	rc = __vcpu_run(vcpu);

	if (signal_pending(current) && !rc) {
		kvm_run->exit_reason = KVM_EXIT_INTR;
		rc = -EINTR;
	}

	if (guestdbg_exit_pending(vcpu) && !rc)  {
		kvm_s390_prepare_debug_exit(vcpu);
		rc = 0;
	}

	if (rc == -EREMOTE) {
		/* userspace support is needed, kvm_run has been prepared */
		rc = 0;
	}

	disable_cpu_timer_accounting(vcpu);
	store_regs(vcpu, kvm_run);

	kvm_sigset_deactivate(vcpu);

	vcpu->stat.exit_userspace++;
out:
	vcpu_put(vcpu);
	return rc;
}