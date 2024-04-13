void kvm_arch_vcpu_block_finish(struct kvm_vcpu *vcpu)
{
	vcpu->valid_wakeup = false;
}