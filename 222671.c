int kvm_s390_vcpu_setup_cmma(struct kvm_vcpu *vcpu)
{
	vcpu->arch.sie_block->cbrlo = get_zeroed_page(GFP_KERNEL);
	if (!vcpu->arch.sie_block->cbrlo)
		return -ENOMEM;
	return 0;
}