bool kvm_s390_vcpu_sie_inhibited(struct kvm_vcpu *vcpu)
{
	return atomic_read(&vcpu->arch.sie_block->prog20) &
	       (PROG_BLOCK_SIE | PROG_REQUEST);
}