static u32 *avic_get_logical_id_entry(struct kvm_vcpu *vcpu, u32 ldr, bool flat)
{
	struct kvm_svm *kvm_svm = to_kvm_svm(vcpu->kvm);
	int index;
	u32 *logical_apic_id_table;
	int dlid = GET_APIC_LOGICAL_ID(ldr);

	if (!dlid)
		return NULL;

	if (flat) { /* flat */
		index = ffs(dlid) - 1;
		if (index > 7)
			return NULL;
	} else { /* cluster */
		int cluster = (dlid & 0xf0) >> 4;
		int apic = ffs(dlid & 0x0f) - 1;

		if ((apic < 0) || (apic > 7) ||
		    (cluster >= 0xf))
			return NULL;
		index = (cluster << 2) + apic;
	}

	logical_apic_id_table = (u32 *) page_address(kvm_svm->avic_logical_id_table_page);

	return &logical_apic_id_table[index];
}