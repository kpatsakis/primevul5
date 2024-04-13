static int avic_init_vcpu(struct vcpu_svm *svm)
{
	int ret;

	if (!kvm_vcpu_apicv_active(&svm->vcpu))
		return 0;

	ret = avic_init_backing_page(&svm->vcpu);
	if (ret)
		return ret;

	INIT_LIST_HEAD(&svm->ir_list);
	spin_lock_init(&svm->ir_list_lock);
	svm->dfr_reg = APIC_DFR_FLAT;

	return ret;
}