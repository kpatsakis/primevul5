static void kvm_s390_vcpu_request_handled(struct kvm_vcpu *vcpu)
{
	atomic_andnot(PROG_REQUEST, &vcpu->arch.sie_block->prog20);
}