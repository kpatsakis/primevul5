static int avic_handle_ldr_update(struct kvm_vcpu *vcpu)
{
	int ret = 0;
	struct vcpu_svm *svm = to_svm(vcpu);
	u32 ldr = kvm_lapic_get_reg(vcpu->arch.apic, APIC_LDR);
	u32 id = kvm_xapic_id(vcpu->arch.apic);

	if (ldr == svm->ldr_reg)
		return 0;

	avic_invalidate_logical_id_entry(vcpu);

	if (ldr)
		ret = avic_ldr_write(vcpu, id, ldr);

	if (!ret)
		svm->ldr_reg = ldr;

	return ret;
}