static void kvm_s390_vcpu_request(struct kvm_vcpu *vcpu)
{
	atomic_or(PROG_REQUEST, &vcpu->arch.sie_block->prog20);
	exit_sie(vcpu);
}