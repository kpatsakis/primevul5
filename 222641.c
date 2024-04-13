static int kvm_s390_vcpu_setup(struct kvm_vcpu *vcpu)
{
	int rc = 0;
	u16 uvrc, uvrrc;

	atomic_set(&vcpu->arch.sie_block->cpuflags, CPUSTAT_ZARCH |
						    CPUSTAT_SM |
						    CPUSTAT_STOPPED);

	if (test_kvm_facility(vcpu->kvm, 78))
		kvm_s390_set_cpuflags(vcpu, CPUSTAT_GED2);
	else if (test_kvm_facility(vcpu->kvm, 8))
		kvm_s390_set_cpuflags(vcpu, CPUSTAT_GED);

	kvm_s390_vcpu_setup_model(vcpu);

	/* pgste_set_pte has special handling for !MACHINE_HAS_ESOP */
	if (MACHINE_HAS_ESOP)
		vcpu->arch.sie_block->ecb |= ECB_HOSTPROTINT;
	if (test_kvm_facility(vcpu->kvm, 9))
		vcpu->arch.sie_block->ecb |= ECB_SRSI;
	if (test_kvm_facility(vcpu->kvm, 73))
		vcpu->arch.sie_block->ecb |= ECB_TE;

	if (test_kvm_facility(vcpu->kvm, 8) && vcpu->kvm->arch.use_pfmfi)
		vcpu->arch.sie_block->ecb2 |= ECB2_PFMFI;
	if (test_kvm_facility(vcpu->kvm, 130))
		vcpu->arch.sie_block->ecb2 |= ECB2_IEP;
	vcpu->arch.sie_block->eca = ECA_MVPGI | ECA_PROTEXCI;
	if (sclp.has_cei)
		vcpu->arch.sie_block->eca |= ECA_CEI;
	if (sclp.has_ib)
		vcpu->arch.sie_block->eca |= ECA_IB;
	if (sclp.has_siif)
		vcpu->arch.sie_block->eca |= ECA_SII;
	if (sclp.has_sigpif)
		vcpu->arch.sie_block->eca |= ECA_SIGPI;
	if (test_kvm_facility(vcpu->kvm, 129)) {
		vcpu->arch.sie_block->eca |= ECA_VX;
		vcpu->arch.sie_block->ecd |= ECD_HOSTREGMGMT;
	}
	if (test_kvm_facility(vcpu->kvm, 139))
		vcpu->arch.sie_block->ecd |= ECD_MEF;
	if (test_kvm_facility(vcpu->kvm, 156))
		vcpu->arch.sie_block->ecd |= ECD_ETOKENF;
	if (vcpu->arch.sie_block->gd) {
		vcpu->arch.sie_block->eca |= ECA_AIV;
		VCPU_EVENT(vcpu, 3, "AIV gisa format-%u enabled for cpu %03u",
			   vcpu->arch.sie_block->gd & 0x3, vcpu->vcpu_id);
	}
	vcpu->arch.sie_block->sdnxo = ((unsigned long) &vcpu->run->s.regs.sdnx)
					| SDNXC;
	vcpu->arch.sie_block->riccbd = (unsigned long) &vcpu->run->s.regs.riccb;

	if (sclp.has_kss)
		kvm_s390_set_cpuflags(vcpu, CPUSTAT_KSS);
	else
		vcpu->arch.sie_block->ictl |= ICTL_ISKE | ICTL_SSKE | ICTL_RRBE;

	if (vcpu->kvm->arch.use_cmma) {
		rc = kvm_s390_vcpu_setup_cmma(vcpu);
		if (rc)
			return rc;
	}
	hrtimer_init(&vcpu->arch.ckc_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	vcpu->arch.ckc_timer.function = kvm_s390_idle_wakeup;

	vcpu->arch.sie_block->hpid = HPID_KVM;

	kvm_s390_vcpu_crypto_setup(vcpu);

	mutex_lock(&vcpu->kvm->lock);
	if (kvm_s390_pv_is_protected(vcpu->kvm)) {
		rc = kvm_s390_pv_create_cpu(vcpu, &uvrc, &uvrrc);
		if (rc)
			kvm_s390_vcpu_unsetup_cmma(vcpu);
	}
	mutex_unlock(&vcpu->kvm->lock);

	return rc;
}