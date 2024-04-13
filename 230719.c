static void avic_init_vmcb(struct vcpu_svm *svm)
{
	struct vmcb *vmcb = svm->vmcb;
	struct kvm_svm *kvm_svm = to_kvm_svm(svm->vcpu.kvm);
	phys_addr_t bpa = __sme_set(page_to_phys(svm->avic_backing_page));
	phys_addr_t lpa = __sme_set(page_to_phys(kvm_svm->avic_logical_id_table_page));
	phys_addr_t ppa = __sme_set(page_to_phys(kvm_svm->avic_physical_id_table_page));

	vmcb->control.avic_backing_page = bpa & AVIC_HPA_MASK;
	vmcb->control.avic_logical_id = lpa & AVIC_HPA_MASK;
	vmcb->control.avic_physical_id = ppa & AVIC_HPA_MASK;
	vmcb->control.avic_physical_id |= AVIC_MAX_PHYSICAL_ID_COUNT;
	if (kvm_apicv_activated(svm->vcpu.kvm))
		vmcb->control.int_ctl |= AVIC_ENABLE_MASK;
	else
		vmcb->control.int_ctl &= ~AVIC_ENABLE_MASK;
}