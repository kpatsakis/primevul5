void kvm_arch_vcpu_postcreate(struct kvm_vcpu *vcpu)
{
	mutex_lock(&vcpu->kvm->lock);
	preempt_disable();
	vcpu->arch.sie_block->epoch = vcpu->kvm->arch.epoch;
	vcpu->arch.sie_block->epdx = vcpu->kvm->arch.epdx;
	preempt_enable();
	mutex_unlock(&vcpu->kvm->lock);
	if (!kvm_is_ucontrol(vcpu->kvm)) {
		vcpu->arch.gmap = vcpu->kvm->arch.gmap;
		sca_add_vcpu(vcpu);
	}
	if (test_kvm_facility(vcpu->kvm, 74) || vcpu->kvm->arch.user_instr0)
		vcpu->arch.sie_block->ictl |= ICTL_OPEREXC;
	/* make vcpu_load load the right gmap on the first trigger */
	vcpu->arch.enabled_gmap = vcpu->arch.gmap;
}