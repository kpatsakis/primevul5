static int kvm_s390_handle_requests(struct kvm_vcpu *vcpu)
{
retry:
	kvm_s390_vcpu_request_handled(vcpu);
	if (!kvm_request_pending(vcpu))
		return 0;
	/*
	 * We use MMU_RELOAD just to re-arm the ipte notifier for the
	 * guest prefix page. gmap_mprotect_notify will wait on the ptl lock.
	 * This ensures that the ipte instruction for this request has
	 * already finished. We might race against a second unmapper that
	 * wants to set the blocking bit. Lets just retry the request loop.
	 */
	if (kvm_check_request(KVM_REQ_MMU_RELOAD, vcpu)) {
		int rc;
		rc = gmap_mprotect_notify(vcpu->arch.gmap,
					  kvm_s390_get_prefix(vcpu),
					  PAGE_SIZE * 2, PROT_WRITE);
		if (rc) {
			kvm_make_request(KVM_REQ_MMU_RELOAD, vcpu);
			return rc;
		}
		goto retry;
	}

	if (kvm_check_request(KVM_REQ_TLB_FLUSH, vcpu)) {
		vcpu->arch.sie_block->ihcpu = 0xffff;
		goto retry;
	}

	if (kvm_check_request(KVM_REQ_ENABLE_IBS, vcpu)) {
		if (!ibs_enabled(vcpu)) {
			trace_kvm_s390_enable_disable_ibs(vcpu->vcpu_id, 1);
			kvm_s390_set_cpuflags(vcpu, CPUSTAT_IBS);
		}
		goto retry;
	}

	if (kvm_check_request(KVM_REQ_DISABLE_IBS, vcpu)) {
		if (ibs_enabled(vcpu)) {
			trace_kvm_s390_enable_disable_ibs(vcpu->vcpu_id, 0);
			kvm_s390_clear_cpuflags(vcpu, CPUSTAT_IBS);
		}
		goto retry;
	}

	if (kvm_check_request(KVM_REQ_ICPT_OPEREXC, vcpu)) {
		vcpu->arch.sie_block->ictl |= ICTL_OPEREXC;
		goto retry;
	}

	if (kvm_check_request(KVM_REQ_START_MIGRATION, vcpu)) {
		/*
		 * Disable CMM virtualization; we will emulate the ESSA
		 * instruction manually, in order to provide additional
		 * functionalities needed for live migration.
		 */
		vcpu->arch.sie_block->ecb2 &= ~ECB2_CMMA;
		goto retry;
	}

	if (kvm_check_request(KVM_REQ_STOP_MIGRATION, vcpu)) {
		/*
		 * Re-enable CMM virtualization if CMMA is available and
		 * CMM has been used.
		 */
		if ((vcpu->kvm->arch.use_cmma) &&
		    (vcpu->kvm->mm->context.uses_cmm))
			vcpu->arch.sie_block->ecb2 |= ECB2_CMMA;
		goto retry;
	}

	/* nothing to do, just clear the request */
	kvm_clear_request(KVM_REQ_UNHALT, vcpu);
	/* we left the vsie handler, nothing to do, just clear the request */
	kvm_clear_request(KVM_REQ_VSIE_RESTART, vcpu);

	return 0;
}