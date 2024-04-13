int kvm_arch_vcpu_ioctl_set_mpstate(struct kvm_vcpu *vcpu,
				    struct kvm_mp_state *mp_state)
{
	int rc = 0;

	vcpu_load(vcpu);

	/* user space knows about this interface - let it control the state */
	vcpu->kvm->arch.user_cpu_state_ctrl = 1;

	switch (mp_state->mp_state) {
	case KVM_MP_STATE_STOPPED:
		rc = kvm_s390_vcpu_stop(vcpu);
		break;
	case KVM_MP_STATE_OPERATING:
		rc = kvm_s390_vcpu_start(vcpu);
		break;
	case KVM_MP_STATE_LOAD:
		if (!kvm_s390_pv_cpu_is_protected(vcpu)) {
			rc = -ENXIO;
			break;
		}
		rc = kvm_s390_pv_set_cpu_state(vcpu, PV_CPU_STATE_OPR_LOAD);
		break;
	case KVM_MP_STATE_CHECK_STOP:
		/* fall through - CHECK_STOP and LOAD are not supported yet */
	default:
		rc = -ENXIO;
	}

	vcpu_put(vcpu);
	return rc;
}