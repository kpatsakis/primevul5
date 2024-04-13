static inline void avic_post_state_restore(struct kvm_vcpu *vcpu)
{
	if (avic_handle_apic_id_update(vcpu) != 0)
		return;
	avic_handle_dfr_update(vcpu);
	avic_handle_ldr_update(vcpu);
}