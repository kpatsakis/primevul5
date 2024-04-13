static void kvm_arch_vcpu_ioctl_normal_reset(struct kvm_vcpu *vcpu)
{
	vcpu->arch.sie_block->gpsw.mask &= ~PSW_MASK_RI;
	vcpu->arch.pfault_token = KVM_S390_PFAULT_TOKEN_INVALID;
	memset(vcpu->run->s.regs.riccb, 0, sizeof(vcpu->run->s.regs.riccb));

	kvm_clear_async_pf_completion_queue(vcpu);
	if (!kvm_s390_user_cpu_state_ctrl(vcpu->kvm))
		kvm_s390_vcpu_stop(vcpu);
	kvm_s390_clear_local_irqs(vcpu);
}