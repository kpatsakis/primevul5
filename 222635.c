void kvm_s390_vcpu_block(struct kvm_vcpu *vcpu)
{
	atomic_or(PROG_BLOCK_SIE, &vcpu->arch.sie_block->prog20);
	exit_sie(vcpu);
}