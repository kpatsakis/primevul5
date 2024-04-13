void kvm_s390_vcpu_unsetup_cmma(struct kvm_vcpu *vcpu)
{
	free_page(vcpu->arch.sie_block->cbrlo);
	vcpu->arch.sie_block->cbrlo = 0;
}