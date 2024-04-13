static int svm_vm_init(struct kvm *kvm)
{
	if (avic) {
		int ret = avic_vm_init(kvm);
		if (ret)
			return ret;
	}

	kvm_apicv_init(kvm, avic);
	return 0;
}