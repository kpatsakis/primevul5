void kvm_s390_vcpu_unblock(struct kvm_vcpu *vcpu)
{
	atomic_andnot(PROG_BLOCK_SIE, &vcpu->arch.sie_block->prog20);
}