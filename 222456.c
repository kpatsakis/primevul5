int kvm_arch_vcpu_ioctl_get_mpstate(struct kvm_vcpu *vcpu,
				    struct kvm_mp_state *mp_state)
{
	int ret;

	vcpu_load(vcpu);

	/* CHECK_STOP and LOAD are not supported yet */
	ret = is_vcpu_stopped(vcpu) ? KVM_MP_STATE_STOPPED :
				      KVM_MP_STATE_OPERATING;

	vcpu_put(vcpu);
	return ret;
}