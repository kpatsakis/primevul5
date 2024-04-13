static void kvm_s390_vcpu_setup_model(struct kvm_vcpu *vcpu)
{
	struct kvm_s390_cpu_model *model = &vcpu->kvm->arch.model;

	vcpu->arch.sie_block->ibc = model->ibc;
	if (test_kvm_facility(vcpu->kvm, 7))
		vcpu->arch.sie_block->fac = (u32)(u64) model->fac_list;
}