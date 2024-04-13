static int avic_init_backing_page(struct kvm_vcpu *vcpu)
{
	u64 *entry, new_entry;
	int id = vcpu->vcpu_id;
	struct vcpu_svm *svm = to_svm(vcpu);

	if (id >= AVIC_MAX_PHYSICAL_ID_COUNT)
		return -EINVAL;

	if (!svm->vcpu.arch.apic->regs)
		return -EINVAL;

	if (kvm_apicv_activated(vcpu->kvm)) {
		int ret;

		ret = avic_update_access_page(vcpu->kvm, true);
		if (ret)
			return ret;
	}

	svm->avic_backing_page = virt_to_page(svm->vcpu.arch.apic->regs);

	/* Setting AVIC backing page address in the phy APIC ID table */
	entry = avic_get_physical_id_entry(vcpu, id);
	if (!entry)
		return -EINVAL;

	new_entry = __sme_set((page_to_phys(svm->avic_backing_page) &
			      AVIC_PHYSICAL_ID_ENTRY_BACKING_PAGE_MASK) |
			      AVIC_PHYSICAL_ID_ENTRY_VALID_MASK);
	WRITE_ONCE(*entry, new_entry);

	svm->avic_physical_id_cache = entry;

	return 0;
}